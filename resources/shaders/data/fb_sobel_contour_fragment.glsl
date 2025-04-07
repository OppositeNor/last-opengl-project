#version 460 core

in VS_OUT {
    vec2 uv;
} vert_out;

out vec4 color;

uniform sampler2D depth_map;
uniform sampler2D normal_map;

mat3 sobel_core_x = mat3 (
     1.0,  2.0,  1.0,
     0.0,  0.0,  0.0,
    -1.0, -2.0, -1.0
);  // GLSL uses column major order


mat3 sobel_core_y = mat3(
    1.0, 0.0, -1.0,
    2.0, 0.0, -2.0,
    1.0, 0.0, -1.0
);

#define NORMAL_MULTIPLIER 1.0
#define DEPTH_MULTIPLIER 1.0

float sobel_edge_normal(sampler2D texture_map);
float sobel_edge_depth(sampler2D texture_map);

void main() {
    float edge_color = NORMAL_MULTIPLIER * sobel_edge_normal(normal_map) + DEPTH_MULTIPLIER * sobel_edge_depth(depth_map);
    color = vec4(vec3(edge_color), 1.0);
}

float get_color_avrg(vec3 p_color) {
    return (p_color.r + p_color.g + p_color.b) / 3.0;
}

float sobel_edge_normal(sampler2D texture_map) {
    float G_x = 0.0;
    float G_y = 0.0;
    vec2 texel_size = 1.0 / textureSize(texture_map, 0);
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            float color_avrg = get_color_avrg(texture(texture_map, vert_out.uv + vec2(i * texel_size.x, j * texel_size.y)).rgb);
            G_x += color_avrg * sobel_core_x[i + 1][j + 1];
            G_y += color_avrg * sobel_core_y[i + 1][j + 1];
        }
    }
    return sqrt(G_x * G_x + G_y * G_y);
}

float sobel_edge_depth(sampler2D texture_map) {
    float G_x = 0.0;
    float G_y = 0.0;
    vec2 texel_size = 1.0 / textureSize(texture_map, 0);
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            float color_avrg = texture(texture_map, vert_out.uv + vec2(i * texel_size.x, j * texel_size.y)).r;
            G_x += color_avrg * sobel_core_x[i + 1][j + 1];
            G_y += color_avrg * sobel_core_y[i + 1][j + 1];
        }
    }
    return sqrt(G_x * G_x + G_y * G_y);
}
