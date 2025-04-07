#version 460 core

in VS_OUT {
    vec2 uv;
} vert_out;

out vec4 color;

uniform sampler2D framebuffer;

void main() {
    float value = texture(framebuffer, vert_out.uv).r;
    color = vec4(value, value, value, 1.0);
}
