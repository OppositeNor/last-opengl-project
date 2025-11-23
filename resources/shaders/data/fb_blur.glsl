#version 460 core

in VS_OUT {
    vec2 uv;
} vert_out;

out vec4 color;

uniform sampler2D framebuffer;

mat3 kernel = mat3(
    0.0625, 0.125, 0.0625,
    0.125,  0.25,  0.125,
    0.0625, 0.125, 0.0625
);
#define FILTER_SIZE 3

// float kernel[5][5] = {
//     {1.0/25, 1.0/25, 1.0/25, 1.0/25, 1.0/25},
//     {1.0/25, 1.0/25, 1.0/25, 1.0/25, 1.0/25},
//     {1.0/25, 1.0/25, 1.0/25, 1.0/25, 1.0/25},
//     {1.0/25, 1.0/25, 1.0/25, 1.0/25, 1.0/25},
//     {1.0/25, 1.0/25, 1.0/25, 1.0/25, 1.0/25}
// };
// #define FILTER_SIZE 5

// mat3 kernel = mat3(
//     1, 1, 1,
//     1, 1, 1,
//     1, 1, 1
// ) / 9;
// #define FILTER_SIZE 3

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
    color = vec4(0.0);
    vec2 texel_size = 1.0 / textureSize(framebuffer, 0);
    for (int i = -FILTER_SIZE / 2; i <= FILTER_SIZE / 2; ++i) {
        for (int j = -FILTER_SIZE / 2; j <= FILTER_SIZE / 2; ++j) {
            color += kernel[i + FILTER_SIZE / 2][j + FILTER_SIZE / 2] * texture(framebuffer, vert_out.uv + vec2(i * texel_size.x, j * texel_size.y));
        }
    }
    // gamma correction
    color.rgb = vec3(correct(color.r), correct(color.g), correct(color.b));
}
