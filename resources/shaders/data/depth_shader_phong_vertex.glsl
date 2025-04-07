#version 460 core

layout(location = VERTEX_LOCATION_POS) in vec3 vert_pos;
layout(location = VERTEX_LOCATION_UV) in vec2 vert_uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out VS_OUT {
    vec4 pos;
    vec2 uv;
} vert_out;

void main() {
    vert_out.pos = view * model * vec4(vert_pos, 1.0);
    vert_out.uv = vert_uv;
    gl_Position = proj * vert_out.pos;
}
