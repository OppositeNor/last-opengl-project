#version 460 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uv;

out VS_OUT {
    vec2 uv;
} vert_out;

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    vert_out.uv = uv;
}
