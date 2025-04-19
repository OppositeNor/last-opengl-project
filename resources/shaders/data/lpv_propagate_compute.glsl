#version 460 core

layout(local_size_x = 4, local_size_y = 4, local_size_z = 4) in;

uniform layout(binding=0,rgba32f) image3D lpv_voxels_r;
uniform layout(binding=0,rgba32f) image3D lpv_voxels_g;
uniform layout(binding=0,rgba32f) image3D lpv_voxels_b;

void propagate_lpv(vec3 position, vec3 direction, vec3 color) {
    
}

void main() {

}

