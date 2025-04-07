#version 460 core

layout(location = VERTEX_LOCATION_POS) in vec3 vert_pos;
layout(location = VERTEX_LOCATION_NORMAL) in vec3 vert_normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out VS_OUT {
    vec3 pos;
} vert_out;

void main() {
    gl_Position = model * vec4(vert_pos, 1.0);
    vert_out.pos = gl_Position.xyz / gl_Position.w;
    gl_Position = proj * view * gl_Position;
}
