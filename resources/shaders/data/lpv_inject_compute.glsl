#version 460 core

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

uniform int spot_light_count;
uniform sampler2D shadow_maps[SpotLight::MAX_LIGHTS];
uniform sampler2D normal_maps[SpotLight::MAX_LIGHTS];
uniform sampler2D coordinate_maps[SpotLight::MAX_LIGHTS];
uniform sampler2D flux_maps[SpotLight::MAX_LIGHTS];
uniform bool lighting_enabled;
uniform bool gi_enabled;

// LPV buffer
layout(std430, binding=LPV::BIND_POINT) buffer LPV {
    float sh_coefs[];
} lpv;
uniform uint lpv_sh_degree;
uniform ivec3 lpv_dimension;
uniform float lpv_voxel_size;
uniform vec3 lpv_lbb;

float flatten(ivec3 p_index, ivec3 p_dimension);
ivec2 reshape_vec2(int p_index, ivec2 p_dimension);
float get_sh_coef(vec3 p_color, vec3 p_normal, int p_sh_degree, int p_sh_index);

int lpv_stride = (LPV::SH_DEGREE + 1) * (LPV::SH_DEGREE + 1) * 3;

#define SH_0    0.28209479177387814
#define SH_1_0  0.4886025119029199
#define SH_1_1  0.3454941494713355
#define SH_2_0 

void main() {
    uint image_index = gl_WorkGroupID.x;
    if (image_index > spot_light_count) {
        return;
    }
    ivec2 task_count = textureSize(flux_maps[image_index], 0).xy;
    task_count.x = int(ceil(float(task_count.x) / 8));
    task_count.y = int(ceil(float(task_count.y) / 8));
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

vec3 eval_sh(int p_index, vec3 p_normal, int p_sh_degree) {

    return vec3(0.0); // TODO
}
