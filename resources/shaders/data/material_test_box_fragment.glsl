#version 460 core

struct CameraF {
    vec3 pos;
};

uniform CameraF camera_f;

struct PointLight {
    vec3 position;
    vec3 color;
    float far_plane;
    float attenuation;
    samplerCube shadow_map;
};

uniform int point_light_count;
uniform PointLight point_lights[PointLight::MAX_LIGHTS];

in VS_OUT {
    vec3 pos;
    vec3 normal;
    vec2 uv;
} vert_out;

out vec4 color;

void main() {
    color = vec4(vec3(0.0), 1.0);
    int point_light_count_constrained = min(point_light_count, PointLight::MAX_LIGHTS);

    vec3 normal = normalize(vert_out.normal);
    vec3 view_dir = normalize(camera_f.pos - vert_out.pos);
    color = vec4(vec3(texture(point_lights[0].shadow_map, -view_dir).r), 1.0);
}
