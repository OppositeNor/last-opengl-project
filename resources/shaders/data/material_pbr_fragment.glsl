#version 460 core

struct Material {
    int albedo_map_count;
    int normal_map_count;
    int metalic_map_count;
    int roughness_map_count;
    int ao_map_count;
    sampler2D albedo_map[Material::MAX_TEXTURE_COUNT];
    sampler2D normal_map[Material::MAX_TEXTURE_COUNT];
    sampler2D metalic_map[Material::MAX_TEXTURE_COUNT];
    sampler2D roughness_map[Material::MAX_TEXTURE_COUNT];
    sampler2D ao_map[Material::MAX_TEXTURE_COUNT];
};

struct CameraF {
    vec3 pos;
};

uniform CameraF camera_f;

uniform Material material;
uniform vec3 ambient_light;
uniform float far_plane;
uniform sampler2D depth_map;

#define OFFSET_CONST 0.01514

in VS_OUT {
    vec2 uv;
    vec3 pos;
    mat3 TBN;
} vert_out;

struct PointLight {
    vec3 position;
    vec3 color;
    float far_plane;
    float attenuation;
    samplerCube shadow_map;
};

struct SpotLight {
    vec3 position;
    vec3 color;
    float far_plane;
    float attenuation;
    int gi_type;
    float shadow_map_bias;
    mat4 light_view;
    mat4 light_space;
    sampler2D shadow_map;
    sampler2D normal_map;
    sampler2D coordinate_map;
    sampler2D flux_map;
};

uniform int point_light_count;
uniform PointLight point_lights[PointLight::MAX_LIGHTS];
uniform int spot_light_count;
uniform SpotLight spot_lights[SpotLight::MAX_LIGHTS];
uniform bool lighting_enabled;
uniform bool direct_lighting_enabled;
uniform bool gi_enabled;

vec3 albedo;
vec3 normal;
float metalic;
float roughness;
float ao;

out vec4 color;

vec3 BRDF(vec3 w_i, vec3 w_o, vec3 h, vec3 F);

vec3 fresnel_schlick(vec3 h, vec3 w_o, vec3 F_0);

vec4 get_value_if_exists(sampler2D p_texture, int p_size, vec3 p_default) {
    vec4 result;
    if (p_size > 0) {
        result = texture(p_texture, vert_out.uv);
    }
    else {
        result = vec4(p_default, 1.0);
    }
    return result;
}

float shadow_point_light_pcf(PointLight light, float kernel_size, float step_size, float bias);
float visibility_spot_light_pcf(SpotLight light, int filter_size, float bias);
float random(vec2 st);
vec3 get_light_space_pos(SpotLight p_light);
vec3 get_spot_light_rsm_sample_intens(vec3 p_to_sample, vec3 p_sample_flux, vec3 p_sample_normal, float p_length_to_sample);
vec3 pbr_shading(vec3 p_light_pos, vec3 p_light_color, float p_light_attenuation, vec3 p_view_dir);

void main() {
    vec3 view_dir = camera_f.pos - vert_out.pos;
    float depth = texture(depth_map, gl_FragCoord.xy / textureSize(depth_map, 0)).r;
    if (length(view_dir) / far_plane > depth + DEPTH_PREPASS_BIAS) {
        discard;
    }
    view_dir = normalize(view_dir);

    vec4 albedo_temp = get_value_if_exists(material.albedo_map[0], material.albedo_map_count, vec3(0.5));
    if (albedo_temp.a == 0) {
        discard;
    }
    albedo = albedo_temp.rgb;
    normal = normalize(get_value_if_exists(material.normal_map[0], material.normal_map_count, vec3(0.0, 0.0, 1.0)).rgb * 2 - 1);
    normal = normalize(vert_out.TBN * normal);
    metalic = get_value_if_exists(material.metalic_map[0], material.metalic_map_count, vec3(0.5, 0.0, 0.0)).r;
    roughness = get_value_if_exists(material.roughness_map[0], material.roughness_map_count, vec3(0.2, 0.0, 0.0)).r;
    ao = get_value_if_exists(material.ao_map[0], material.ao_map_count, vec3(1.0, 1.0, 1.0)).r;

    color = vec4(albedo * ambient_light, 1.0);
    if (!lighting_enabled) {
        color = vec4(albedo, 1.0);
    }
    else {
        int light_count_constrained = min(point_light_count, PointLight::MAX_LIGHTS);
        for (int i = 0; i < light_count_constrained; ++i) {
            if (direct_lighting_enabled) {
                float shadow = shadow_point_light_pcf(point_lights[i], PointLight::SHADOW_FILTER_SIZE,
                        PointLight::SHADOW_FILTER_SIZE / PointLight::SHADOW_FILTER_COUNT, PointLight::SHADOW_MAP_BIAS);
                if (shadow == 0.0) {
                    continue;
                }
                color += vec4(pbr_shading(point_lights[i].position, point_lights[i].color, point_lights[i].attenuation, view_dir) * shadow, 0.0);
            }
        }
        light_count_constrained = min(spot_light_count, SpotLight::MAX_LIGHTS);
        vec2 fragment_uv = gl_FragCoord.xy / textureSize(depth_map, 0);
        for (int i = 0; i < light_count_constrained; ++i) {
            if (direct_lighting_enabled) {
                float visibility = visibility_spot_light_pcf(spot_lights[i], SpotLight::SHADOW_FILTER_SIZE, spot_lights[i].shadow_map_bias);
                if (visibility != 0.0) {
                    color += vec4(pbr_shading(spot_lights[i].position, spot_lights[i].color, spot_lights[i].attenuation, view_dir) * visibility, 0.0);
                }
            }
            if (gi_enabled && spot_lights[i].gi_type == SpotLight::GIType::RSM) {
                vec3 light_space_pos = get_light_space_pos(spot_lights[i]);
                if (light_space_pos.z < -1 || light_space_pos.z > 1) {
                    continue;
                }
                float xi = random(fragment_uv);
                vec3 sample_pos;
                vec3 sample_flux;
                vec3 sample_normal;
                vec3 light_dir;
                vec2 sample_uv;

                // https://github.com/JuanDiegoMontoya/Fwog/blob/main/example/shaders/rsm/IndirectDitheredFiltered.comp.glsl#L63-L101
                // Samples need to be normalized based on the radius that is sampled, otherwise changing rMax will affect the brightness.
                float r_max = 0.0;
                vec3 total_color = vec3(0.0);

                for (int j = 0; j < SpotLight::RSM_SAMPLE_COUNT; ++j) {
                    float angle = 2 * PI * xi;
                    xi = random(fragment_uv + (j+1) * OFFSET_CONST);
                    float scale = SpotLight::RSM_SAMPLE_RMAX * xi;
                    sample_uv = light_space_pos.xy + vec2(scale * sin(angle), scale * cos(angle));
                    if (sample_uv.x > 1 || sample_uv.x < 0 || sample_uv.y > 1 || sample_uv.y < 0) {
                        continue;
                    }
                    sample_pos = texture(spot_lights[i].coordinate_map, sample_uv).rgb;
                    sample_flux = texture(spot_lights[i].flux_map, sample_uv).rgb;
                    sample_normal = texture(spot_lights[i].normal_map, sample_uv).rgb * 2 - 1;
                    light_dir = sample_pos - vert_out.pos;
                    float length_light_dir = length(light_dir);
                    if (length(light_dir) > r_max) {
                        r_max = length_light_dir;
                    }
                    vec3 E = get_spot_light_rsm_sample_intens(light_dir, sample_flux, sample_normal, length_light_dir) * xi * xi;
                    light_dir = normalize(light_dir);
                    vec3 h = normalize(light_dir + view_dir);
                    vec3 k_s = fresnel_schlick(h, view_dir, mix(vec3(0.4), albedo, metalic));
                    total_color +=  BRDF(light_dir, view_dir, h, k_s) * E;
                }
                color += vec4(total_color * r_max * r_max * 2.0 / SpotLight::RSM_SAMPLE_COUNT, 0.0);
            }
        }
    }
}

vec3 pbr_shading(vec3 p_light_pos, vec3 p_light_color, float p_light_attenuation, vec3 p_view_dir) {
    vec3 w_i = normalize(p_light_pos - vert_out.pos);
    vec3 h = normalize(w_i + p_view_dir);
    vec3 k_s = fresnel_schlick(h, p_view_dir, mix(vec3(0.4), albedo, metalic));
    vec3 k_d = (1.0 - metalic) * (vec3(1.0) - k_s);
    float distance = length(p_light_pos - vert_out.pos);
    vec3 radience = p_light_color / (distance * distance) * p_light_attenuation;
    return radience * (k_d * albedo / PI + BRDF(w_i, p_view_dir, h, k_s)) * ao;
}

float N_TRGGX(vec3 h) {
    float alpha = roughness * roughness;
    float alpha_2 = alpha * alpha;
    float n_dot_h = max(dot(normal, h), 0.0);
    float denom = n_dot_h * n_dot_h * (alpha_2 - 1) + 1;
    return alpha_2 / (PI * denom * denom);
}

float G_SGGX(float n_dot_v, float k) {
    return n_dot_v / (n_dot_v * (1 - k) + k);
}

vec3 fresnel_schlick(vec3 h, vec3 w_o, vec3 F_0) {
    return F_0 + (1.0 - F_0) * pow(1.0 - max(dot(h, w_o), 0.0), 5.0);
}

vec3 BRDF(vec3 w_i, vec3 w_o, vec3 h, vec3 F) {
    float n_dot_w_o = max(dot(normal, w_o), 0.00001);
    float n_dot_w_i = max(dot(normal, w_i), 0.00001);
    float N = N_TRGGX(h);
    float k = (roughness + 1) * (roughness + 1) / 8;
    float G = G_SGGX(n_dot_w_o, k) * G_SGGX(n_dot_w_i, k);
    return N * F * G / (4 * n_dot_w_o * n_dot_w_i);
}

float shadow_point_light_pcf(PointLight light, float kernel_size, float step_size, float bias) {
    // shadow calculation
    float shadow = 0.0f;
    vec3 to_light = light.position - vert_out.pos;
    float distance_to_light = length(to_light);
    for (float x = -kernel_size / 2.0; x < kernel_size / 2.0; x += step_size) {
        for (float y = -kernel_size / 2.0; y < kernel_size / 2.0; y += step_size) {
            for (float z = -kernel_size / 2.0; z < kernel_size / 2.0; z += step_size) {
                float distance = texture(light.shadow_map, -to_light + vec3(x, y, z)).r * light.far_plane;
                if (distance_to_light - bias < distance) {
                    shadow += 1.0;
                }
            }
        }
    }
    return shadow / pow(kernel_size / step_size, 3);
}

float visibility_spot_light_pcf(SpotLight light, int filter_size, float bias) {
    vec4 local_position = light.light_view * vec4(vert_out.pos, 1.0);
    if (local_position.z > 0.0) {
        return 0.0;
    }
    // shadow calculation
    float shadow = 0.0f;
    float distance_to_light = length(local_position.xyz) / light.far_plane;

    // PCF
    vec4 light_space_position = light.light_space * vec4(vert_out.pos, 1.0);
    light_space_position /= light_space_position.w;
    ivec2 map_size = textureSize(light.shadow_map, 0);
    ivec2 pos = ivec2(map_size * (light_space_position.xy + 1) / 2);
    for (int x = -filter_size / 2; x < filter_size / 2; ++x) {
        for (int y = -filter_size / 2; y < filter_size / 2; ++y) {
            ivec2 loc_pos = pos + ivec2(x, y);
            if (loc_pos.x < 0.0 || loc_pos.x >= map_size.x || loc_pos.y < 0.0 || loc_pos.y >= map_size.y) {
                continue;
            }
            float distance = texelFetch(light.shadow_map, loc_pos, 0).r;
            if (distance_to_light - bias < distance) {
                shadow += 1.0;
            }
        }
    }
    return shadow / pow(filter_size, 2);
}

vec3 get_spot_light_rsm_sample_intens(vec3 p_to_sample, vec3 p_sample_flux, vec3 p_sample_normal, float p_length_to_sample) {
    float denom = p_length_to_sample;
    denom *= denom;
    denom *= denom;
    return p_sample_flux * max(0, dot(p_sample_normal, -p_to_sample)) * max(0, dot(normal, p_to_sample)) / denom;
}

float random(vec2 st) {
    // https://thebookofshaders.com/10/
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

vec3 get_light_space_pos(SpotLight p_light) {
    vec4 light_space_pos = p_light.light_space * vec4(vert_out.pos, 1.0);
    return (light_space_pos.xyz / light_space_pos.w + 1.0) / 2.0;
}
