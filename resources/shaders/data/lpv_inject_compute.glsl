#version 460 core

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout(std430, binding=LPV::BIND_POINT) buffer LPV {
    float sh_coefs[];
} lpv;
uniform uint lpv_sh_degree;
uniform ivec3 lpv_dimension;
uniform float lpv_voxel_size;
uniform vec3 lpv_lbb;

// Probably using an ssbo for lights and send them together is a better idea.
struct SpotLight {
    sampler2D normal_map;
    sampler2D coordinate_map;
    sampler2D flux_map;
};

uniform SpotLight spot_light;
float flatten(ivec3 p_index, ivec3 p_dimension);
ivec2 reshape_vec2(int p_index, ivec2 p_dimension);
float get_sh_coef(vec3 p_color, vec3 p_normal, int p_sh_degree, int p_sh_index);

int lpv_stride = (LPV::SH_DEGREE + 1) * (LPV::SH_DEGREE + 1) * 3;

#define SH_0 = 0.28209479177387814
#define SH_1 = 0.28209479177387814

void main() {
    // TODO
}

float flatten(ivec3 p_index, ivec3 p_dimension) {
    // Clamp the index to make sure it does not cause a buffer overflow.
    p_index = clamp(p_index, ivec3(0), p_dimension - ivec3(1));
    return p_index.z * p_dimension.x * p_dimension.y + p_index.y * p_dimension.x + p_index.x;
}

ivec2 reshape_vec2(int p_index, ivec2 p_dimension) {
    p_index = clamp(p_index, 0, p_dimension.x * p_dimension.y - 1);
    return ivec2(p_index / p_dimension.x, p_index % p_dimension.x);
}

float get_sh_coef(vec3 p_color, vec3 p_normal, int p_sh_degree, int p_sh_index) {
    if (p_sh_index > LPV::SH_DEGREE) {
        // Error
        return 0.0f;
    }
    return 0.0f; // TODO
}
