#version 460 core

in VS_OUT {
    vec2 uv;
} vert_out;

out vec4 color;

uniform sampler2D framebuffer;

void main() {
    color = texture(framebuffer, vert_out.uv);
    // gamma correction
    color.rgb = pow(color.rgb, vec3(1.0 / 2.2));
}
