#version 460 core

in VS_OUT {
    vec3 pos;
    vec2 uv;
} vert_out;

out vec4 color;

struct Material {
    int albedo_map_count;
    sampler2D albedo_map[Material::MAX_TEXTURE_COUNT];
};

uniform Material material;

struct SpotLight {
    vec3 color;
    float attenuation;
    sampler2D shadow_map;
};

uniform SpotLight spot_light;

void main() {
    vec3 albedo;
    if (material.albedo_map_count > 0) {
        albedo = texture(material.albedo_map[0], vert_out.uv).rgb;
    }
    else {
        albedo = vec3(1.0);
    }
    float d = length(vert_out.pos);
    float shadow_map_distance = texture(spot_light.shadow_map, vert_out.uv).r;
    if (d - shadow_map_distance < 0) {
        discard;
    }
    vec3 light_flux = spot_light.color * spot_light.attenuation / (d * d);
    color = vec4(light_flux * albedo, 1.0);
}
