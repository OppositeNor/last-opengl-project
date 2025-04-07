#version 460 core

layout(location = VERTEX_LOCATION_POS) in vec3 vert_pos;
layout(location = VERTEX_LOCATION_UV) in vec2 vert_uv;

struct CameraV {
    mat4 view;
};

uniform CameraV camera_v;

uniform mat4 model;
uniform mat4 proj;

out VS_OUT {
    vec2 uv;
} vert_out;

void main() {
    vert_out.uv = vert_uv;
    gl_Position = proj * camera_v.view * model * vec4(vert_pos, 1.0);
}
