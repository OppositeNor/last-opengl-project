#version 460 core

in VS_OUT {
    vec3 pos;
    vec2 uv;
} vert_out;

out vec4 color;

struct Material {
    int diffuse_map_count;
    sampler2D diffuse_map[Material::MAX_TEXTURE_COUNT];
};

uniform Material material;

struct SpotLight {
    vec3 color;
    float attenuation;
};

uniform SpotLight spot_light;

void main() {
    vec3 diffuse;
    if (material.diffuse_map_count > 0) {
        diffuse = texture(material.diffuse_map[0], vert_out.uv).rgb;
    }
    else {
        diffuse = vec3(1.0);
    }
    float d = length(vert_out.pos);
    vec3 light_flux = spot_light.color * spot_light.attenuation / (d * d);
    color = vec4(light_flux * diffuse, 1.0);
}
