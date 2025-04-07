#version 460 core

in VS_OUT {
    vec3 pos;
    vec2 uv;
} vert_out;

out vec4 color;

struct Material {
    vec3 diffuse_color;
};

uniform Material material;

struct SpotLight {
    vec3 color;
    float attenuation;
};

uniform SpotLight spot_light;

void main() {
    float d = length(vert_out.pos);
    vec3 light_flux = spot_light.color * spot_light.attenuation / (d * d);
    color = vec4(light_flux * material.diffuse_color, 1.0);
}
