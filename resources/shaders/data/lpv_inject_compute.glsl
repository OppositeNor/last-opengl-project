#version 460 core

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

uniform vec3 lpv_bounds_lbb; // lpv bound left bottom back
uniform vec3 lpv_bounds_rtf; // lpv bound right top front
uniform vec3 lpv_dimensions; // lpv dimensions

layout(std430, binding=LPV::BIND_POINT) buffer Lights
{
    float sh_coefs[];
} lpv;
uniform uint lpv_sh_degree;
uniform ivec3 lpv_dimension;
uniform float lpv_voxel_size;

struct SpotLight {
    sampler2D normal_map;
    sampler2D coordinate_map;
    sampler2D flux_map;
};

uniform SpotLight spot_light;

float flatten(ivec3 p_index, ivec3 p_dimension);
ivec2 reshape_vec2(float p_index, ivec2 p_dimension);

// https://computergraphics.stackexchange.com/questions/4997/spherical-harmonics-diffuse-cubemap-how-to-get-coefficients
#define SH_0_0(normal) ( 0.282095; )
#define SH_1_N1(normal)( 0.488603 * normal.y; )
#define SH_1_0(normal) ( 0.488603 * normal.z; )
#define SH_1_1(normal) ( 0.488603 * normal.x; )

void main() {
    // TODO
}

float flatten(ivec3 p_index, ivec3 p_dimension) {
    // Clamp the index to make sure it does not cause a buffer overflow.
    p_index = clamp(p_index, ivec3(0), p_dimension - ivec3(1));
    return p_index.z * p_dimension.x * p_dimension.y + p_index.y * p_dimension.x + p_index.x;
}

ivec2 reshape_vec2(float p_index, ivec2 p_dimension) {
    return 
}
