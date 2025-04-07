#version 460 core

struct PointLight {
    vec3 position;
    vec3 color;
    float far_plane;
    float attenuation;
    samplerCube shadow_map;
};

#define OFFSET_CONST 0.01514

struct SpotLight {
    vec3 position;
    vec3 color;
    float far_plane;
    float attenuation;
    float shadow_map_bias;
    int gi_type;
    mat4 light_view;
    mat4 light_space;
    sampler2D shadow_map;
    sampler2D normal_map;
    sampler2D coordinate_map;
    sampler2D flux_map;
};

struct Material {
    int diffuse_map_count;
    int specular_map_count;
    int normal_map_count;
    sampler2D diffuse_map[Material::MAX_TEXTURE_COUNT];
    sampler2D specular_map[Material::MAX_TEXTURE_COUNT];
    sampler2D normal_map[Material::MAX_TEXTURE_COUNT];
};

struct CameraF {
    vec3 pos;
};

uniform CameraF camera_f;

uniform Material material;
uniform vec3 ambient_light;
uniform float far_plane;
uniform sampler2D depth_map;

uniform int point_light_count;
uniform PointLight point_lights[PointLight::MAX_LIGHTS];
uniform int spot_light_count;
uniform SpotLight spot_lights[SpotLight::MAX_LIGHTS];
uniform bool lighting_enabled;
uniform bool direct_lighting_enabled;
uniform bool gi_enabled;

in VS_OUT {
    vec2 uv;
    vec3 pos;
    mat3 TBN;
} vert_out;

out vec4 color;

vec3 get_light_color(vec3 p_normal, vec3 p_diffuse, vec3 p_specular, vec3 p_view_dir, vec3 p_light_pos, vec3 p_light_color, float p_light_atten);

float shadow_point_light(PointLight light);
float visibility_spot_light_pcf(SpotLight light, int filter_size, float bias);
float random(vec2 st);
vec3 get_light_space_pos(SpotLight p_spot_light);
vec3 get_spot_light_rsm_sample_intens(vec3 normal, vec3 p_to_sample, vec3 p_sample_flux, vec3 p_sample_normal, float p_length_to_sample);

void main() {
    vec3 view_dir = camera_f.pos - vert_out.pos;
    float depth = texture(depth_map, gl_FragCoord.xy / textureSize(depth_map, 0)).r;
    if (length(view_dir) / far_plane > depth + DEPTH_PREPASS_BIAS) {
        discard;
    }
    view_dir = normalize(view_dir);
    vec3 diffuse;
    if (material.diffuse_map_count > 0) {
        vec4 diffuse_temp = texture(material.diffuse_map[0], vert_out.uv);
        if (diffuse_temp.a == 0) {
            discard;
        }
        diffuse = diffuse_temp.xyz;
    }
    else {
        diffuse = vec3(1.0);
    }
    vec3 specular;
    if (material.specular_map_count > 0) {
        specular = texture(material.specular_map[0], vert_out.uv).xyz;
    }
    else {
        specular = vec3(0.5);
    }
    vec3 normal;
    if (material.normal_map_count > 0) {
        normal = normalize(texture(material.normal_map[0], vert_out.uv).xyz * 2 - 1);
        normal = normalize(vert_out.TBN * normal);
        return;
    }
    else {
        normal = vert_out.TBN * vec3(0.0, 0.0, 1.0);
    }
    color = vec4(diffuse * ambient_light, 1.0);
    if (lighting_enabled) {
        int light_count_constrained = min(point_light_count, PointLight::MAX_LIGHTS);
        for (int i = 0; i < light_count_constrained; i++) {
            if (direct_lighting_enabled) {
                float visibility = shadow_point_light(point_lights[i]);
                if (visibility != 0.0) {
                    color += vec4(get_light_color(normal, diffuse, specular, view_dir, point_lights[i].position, point_lights[i].color, point_lights[i].attenuation) * visibility, 0.0);
                }
            }
        }
        light_count_constrained = min(spot_light_count, SpotLight::MAX_LIGHTS);
        vec2 fragment_uv = gl_FragCoord.xy / textureSize(depth_map, 0);
        for (int i = 0; i < light_count_constrained; i++) {

            if (direct_lighting_enabled) {
                float visibility = visibility_spot_light_pcf(spot_lights[i], SpotLight::SHADOW_FILTER_SIZE, spot_lights[i].shadow_map_bias);
                if (visibility != 0) {
                    color += vec4(get_light_color(normal, diffuse, specular, view_dir, spot_lights[i].position, spot_lights[i].color, spot_lights[i].attenuation) * visibility, 0.0);
                }
            }
            if (gi_enabled && spot_lights[i].gi_type == SpotLight::GIType::RSM) {
                vec3 light_space_pos = get_light_space_pos(spot_lights[i]);
                if (light_space_pos.z < -1 || light_space_pos.z > 1) {
                    continue;
                }

                float xi = random(fragment_uv);
                // vec3 total_e = vec3(0.0);
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
                    vec2 sample_uv = light_space_pos.xy + vec2(scale * sin(angle), scale * cos(angle));
                    if (sample_uv.x > 1 || sample_uv.x < 0 || sample_uv.y > 1 || sample_uv.y < 0) {
                        continue;
                    }
                    sample_pos = texture(spot_lights[i].coordinate_map, sample_uv).rgb;
                    sample_flux = texture(spot_lights[i].flux_map, sample_uv).rgb;
                    sample_normal = texture(spot_lights[i].normal_map, sample_uv).rgb * 2 - 1;
                    vec3 to_sample = sample_pos - vert_out.pos;
                    float length_to_sample = length(to_sample);
                    if (length_to_sample > r_max) {
                        r_max = length_to_sample;
                    }
                    vec3 E = get_spot_light_rsm_sample_intens(normal, to_sample, sample_flux, sample_normal, length_to_sample);
                    total_color += diffuse * E * xi * xi;
                }
                color += vec4(2.0 * r_max * r_max * total_color / SpotLight::RSM_SAMPLE_COUNT, 0.0);
            }
        }
    }
    else {
        color = vec4(diffuse, 1.0);
    }
}

vec3 get_light_space_pos(SpotLight p_light) {
    vec4 light_space_pos = p_light.light_space * vec4(vert_out.pos, 1.0);
    return ((light_space_pos.xyz / light_space_pos.w) + 1.0) / 2.0;
}

vec3 get_light_color(vec3 p_normal, vec3 p_diffuse, vec3 p_specular, vec3 p_view_dir, vec3 p_light_pos, vec3 p_light_color, float p_light_atten) {
    float distance = length(p_light_pos - vert_out.pos);
    vec3 light_dir = normalize(p_light_pos - vert_out.pos);
    
    float diffuse_intens = max(dot(p_normal, light_dir), 0.0);
    vec3 diffuse_color = p_light_color * diffuse_intens * p_diffuse;

    vec3 reflect_dir = reflect(-light_dir, p_normal);
    float specular_intens = pow(max(dot(p_view_dir, reflect_dir), 0.0), 32);
    vec3 specular_color = p_light_color * specular_intens * p_specular;
    float radience = p_light_atten / (distance * distance);
    return (diffuse_color + specular_color) * radience;
}

vec3 get_spot_light_rsm_sample_intens(vec3 normal, vec3 p_to_sample, vec3 p_sample_flux, vec3 p_sample_normal, float p_length_to_sample) {
    float denom = p_length_to_sample;
    denom *= denom;
    denom *= denom;
    return p_sample_flux * max(0, dot(p_sample_normal, -p_to_sample)) * max(0, dot(normal, p_to_sample)) / denom;
}

float shadow_point_light(PointLight light) {
    // shadow calculation
    float shadow = 0.0f;
    vec3 to_light = light.position - vert_out.pos;
    float distance_to_light = length(to_light);
    for (float x = -PointLight::SHADOW_FILTER_SIZE; x < PointLight::SHADOW_FILTER_SIZE;
            x += PointLight::SHADOW_FILTER_SIZE / (PointLight::SHADOW_FILTER_COUNT * 0.5)) {
        for (float y = -PointLight::SHADOW_FILTER_SIZE; y < PointLight::SHADOW_FILTER_SIZE;
                y += PointLight::SHADOW_FILTER_SIZE / (PointLight::SHADOW_FILTER_COUNT * 0.5)) {
            for (float z = -PointLight::SHADOW_FILTER_SIZE; z < PointLight::SHADOW_FILTER_SIZE;
                    z += PointLight::SHADOW_FILTER_SIZE / (PointLight::SHADOW_FILTER_COUNT * 0.5)) {
                float distance = texture(light.shadow_map, -to_light + vec3(x, y, z)).r * light.far_plane;
                if (distance_to_light - PointLight::SHADOW_MAP_BIAS < distance) {
                    shadow += 1.0;
                }
            }
        }
    }
    return shadow / pow(PointLight::SHADOW_FILTER_COUNT, 3);
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

float random(vec2 st) {
    // https://thebookofshaders.com/10/
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}
