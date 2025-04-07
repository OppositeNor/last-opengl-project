#version 460 core

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

uniform layout(binding=0,rgba32f) image3D lpv_voxels_r;
uniform layout(binding=1,rgba32f) image3D lpv_voxels_g;
uniform layout(binding=2,rgba32f) image3D lpv_voxels_b;

uniform vec3 lpv_bounds_lbb; // lpv bound left bottom back
uniform vec3 lpv_bounds_rtf; // lpv bound right top front
uniform vec3 lpv_dimensions; // lpv dimensions

struct SpotLight {
    sampler2D normal_map;
    sampler2D coordinate_map;
    sampler2D flux_map;
};

uniform SpotLight spot_light;

// https://computergraphics.stackexchange.com/questions/4997/spherical-harmonics-diffuse-cubemap-how-to-get-coefficients
float sh_0_0(vec3 normal) { return 0.282095; }
float sh_1_n1(vec3 normal) { return 0.488603 * normal.y; }
float sh_1_0(vec3 normal) { return 0.488603 * normal.z; }
float sh_1_1(vec3 normal) { return 0.488603 * normal.y; }

void main() {
    ivec2 rsm_size = textureSize(spot_light.flux_map, 0);
    if (any(greaterThanEqual(gl_GlobalInvocationID.xy, rsm_size.xy))) {
        return;
    }
    vec2 rsm_uv = vec2(gl_GlobalInvocationID.xy) / vec2(rsm_size);
    vec3 coord = texture(spot_light.coordinate_map, rsm_uv).xyz;
    vec3 lpv_uv = (coord - lpv_bounds_lbb) / (lpv_bounds_rtf - lpv_bounds_lbb);
    if (any(greaterThan(lpv_uv, vec3(1.0))) || any(lessThan(lpv_uv, vec3(0.0)))) {
        return;
    }
    ivec3 lpv_index = ivec3(floor(lpv_uv * lpv_dimensions));
    vec3 normal = texture(spot_light.normal_map, rsm_uv).xyz * 2 - 1;
    vec3 flux = texture(spot_light.flux_map, rsm_uv).xyz;

    vec3 sh_result_0_0 = sh_0_0(normal) * flux;
    vec3 sh_result_1_n1 = sh_1_n1(normal) * flux;
    vec3 sh_result_1_0 = sh_1_0(normal) * flux;
    vec3 sh_result_1_1 = sh_1_1(normal) * flux;

    imageStore(lpv_voxels_r, lpv_index,
            vec4(sh_result_0_0.r,
                sh_result_1_n1.r,
                sh_result_1_0.r,
                sh_result_1_1.r));
    imageStore(lpv_voxels_r, lpv_index,
            vec4(sh_result_0_0.g,
                sh_result_1_n1.g,
                sh_result_1_0.g,
                sh_result_1_1.g));
    imageStore(lpv_voxels_r, lpv_index,
            vec4(sh_result_0_0.b,
                sh_result_1_n1.b,
                sh_result_1_0.b,
                sh_result_1_1.b));
}

