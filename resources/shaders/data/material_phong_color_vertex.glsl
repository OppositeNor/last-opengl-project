#version 460 core

layout(location = VERTEX_LOCATION_POS) in vec3 vert_pos;
layout(location = VERTEX_LOCATION_NORMAL) in vec3 vert_normal;
layout(location = VERTEX_LOCATION_UV) in vec2 vert_uv;
layout(location = VERTEX_LOCATION_TANGENT) in vec3 vert_tangent;

struct CameraV {
    mat4 view;
};

uniform CameraV camera_v;

uniform mat4 model;
uniform mat4 proj;

out VS_OUT {
    vec3 pos;
    vec3 normal;
    vec2 uv;
} vert_out;

void main() {
    vert_out.uv = vert_uv;
    vert_out.normal = mat3(transpose(inverse(model))) * vert_normal;
    vert_out.pos = vec3(model * vec4(vert_pos, 1.0));
    gl_Position = proj * camera_v.view * model * vec4(vert_pos, 1.0);
}
