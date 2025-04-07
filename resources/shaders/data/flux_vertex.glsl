#version 460 core

layout(location = VERTEX_LOCATION_POS) in vec3 vert_pos;
layout(location = VERTEX_LOCATION_NORMAL) in vec3 vert_normal;
layout(location = VERTEX_LOCATION_UV) in vec2 vert_uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out VS_OUT {
    vec3 pos;
    vec2 uv;
} vert_out;

void main() {
    gl_Position = view * model * vec4(vert_pos, 1.0);
    vert_out.pos = gl_Position.xyz;
    gl_Position = proj * gl_Position;
    vert_out.uv = vert_uv;
}
