#version 460 core

in VS_OUT {
    vec2 uv;
} vert_out;

out vec4 color;

uniform sampler2D framebuffer;

// linear RGB -> sRGB
float correct(float p_cl) {
    if (p_cl >= 1.0) {
        return 1.0;
    } else if (p_cl <= 0.0) {
        return 0.0;
    } else if (p_cl < 0.0031308) {
        return 12.92 * p_cl;
    } else {
        return 1.055 * pow(p_cl, 0.41666) - 0.055;
    }
}

void main() {
    color = texture(framebuffer, vert_out.uv);
    // gamma correction
    color.rgb = vec3(correct(color.r), correct(color.g), correct(color.b));
}
