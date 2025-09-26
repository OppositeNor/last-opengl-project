#include "voxel/voxel_lpv.h"
#include "graphics.h"
#include <memory>

VoxelLPV::VoxelLPV(glm::vec3 p_dimension, size_t p_voxel_size)
    : Voxel(p_dimension, p_voxel_size) {
    voxels = std::make_unique<VoxelData[]>(dimension.x * dimension.y * dimension.z);
}

VoxelLPV::~VoxelLPV() {
    if (ssbo != 0) {
        GL_CALL(glDeleteBuffers, 1, &ssbo);
    }
}

void VoxelLPV::bind(uint32_t p_index) const {
    GL_CALL(glBindBufferBase, GL_SHADER_STORAGE_BUFFER, p_index, get_ssbo());
}

void VoxelLPV::unbind(uint32_t p_index) const {
    GL_CALL(glBindBufferBase, GL_SHADER_STORAGE_BUFFER, p_index, 0);
}


uint32_t VoxelLPV::get_ssbo() const {
    if (ssbo == 0) {
        init_ssbo();
    }
    return ssbo;
}

void VoxelLPV::init_ssbo() const {
    GL_CALL(glGenBuffers, 1, &ssbo);
    GL_CALL(glBindBuffer, GL_SHADER_STORAGE_BUFFER, ssbo);
    GL_CALL(glBufferData, GL_SHADER_STORAGE_BUFFER, dimension.x * dimension.y * dimension.z * sizeof(VoxelData),
            voxels.get(), GL_DYNAMIC_READ);
    GL_CALL(glBindBuffer, GL_SHADER_STORAGE_BUFFER, 0);
}
