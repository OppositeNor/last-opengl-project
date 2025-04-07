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

void main() {
    color = vec4(0.0);
    vec2 texel_size = 1.0 / textureSize(framebuffer, 0);
    for (int i = -FILTER_SIZE / 2; i <= FILTER_SIZE / 2; ++i) {
        for (int j = -FILTER_SIZE / 2; j <= FILTER_SIZE / 2; ++j) {
            color += kernel[i + FILTER_SIZE / 2][j + FILTER_SIZE / 2] * texture(framebuffer, vert_out.uv + vec2(i * texel_size.x, j * texel_size.y));
        }
    }
    // gamma correction
    color.rgb = pow(color.rgb, vec3(1.0 / 2.2));
}
