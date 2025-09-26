#ifndef __WBE_VOXEL_LPV_H__
#define __WBE_VOXEL_LPV_H__

#include "voxel.hpp"
#include <cstdint>
#include <glm/ext/vector_float3.hpp>
#include <memory>
class VoxelLPV : public Voxel {
public:
    VoxelLPV(glm::vec3 p_dimension, size_t p_voxel_size);
    virtual ~VoxelLPV();

    virtual void bind(uint32_t p_index) const override;
    virtual void unbind(uint32_t p_index) const override;
    virtual void set_uniform(const Shader* p_shader) const override;

    static constexpr uint32_t BIND_POINT = 1;
    static constexpr int SH_DEGREE = 9;

private:
    struct VoxelData {
        // Spherical harmonics coefficients.
        float sh_coef[9];
    };

    // Flattens a vec3 index to the index of the voxel.
    uint32_t flatten(glm::vec3 p_index) {
        return p_index.z * dimension.x * dimension.y + p_index.y * dimension.x + p_index.x;
    }

    std::unique_ptr<VoxelData[]> voxels;
    mutable uint32_t ssbo = 0;

    uint32_t get_ssbo() const;
    void init_ssbo() const;
};

#endif
