#version 460 core

layout(local_size_x = 4, local_size_y = 4, local_size_z = 4) in;

uniform layout(binding=0,rgba32f) image3D lpv_voxels;

void main() {
    // TODO
}
