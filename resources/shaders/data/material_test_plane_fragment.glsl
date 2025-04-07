#version 460 core

struct SpotLight {
    vec3 position;
    vec3 color;
    float far_plane;
    float attenuation;
    mat4 light_space;
    sampler2D shadow_map;
    sampler2D normal_map;
    sampler2D coordinate_map;
    sampler2D flux_map;
};

struct Material {
    int light_index;
    int display_map_index;
};

uniform Material material;

uniform int point_light_count;
uniform SpotLight spot_lights[SpotLight::MAX_LIGHTS];

in VS_OUT {
    vec2 uv;
} vert_out;

out vec4 color;

void main() {
    int light_index = clamp(material.light_index, 0, point_light_count - 1);
    int display_map_clamped = clamp(material.display_map_index, 0, 4);
    switch (display_map_clamped) {
        case 0: {
            float val = texture(spot_lights[light_index].shadow_map, vert_out.uv).r;
            color = vec4(val, val, val, 1.0);
        } break;
        case 1:
            color = texture(spot_lights[light_index].normal_map, vert_out.uv);
            break;
        case 2:
            color = vec4(texture(spot_lights[light_index].coordinate_map, vert_out.uv).rgb / 10.0, 1.0);
            break;
        default:
            color = vec4(texture(spot_lights[light_index].flux_map, vert_out.uv).rgb, 1.0);
            break;
    }
}
