#ifndef __VOXEL_H__
#define __VOXEL_H__

#include "shader/shader.h"
#include <cstddef>
#include <cstdlib>
#include <glm/glm.hpp>

class Voxel {
public:
    Voxel(glm::vec3 p_dimension, size_t p_voxel_size)
        : dimension(p_dimension), voxel_size(std::fabs(p_voxel_size)) {}
    virtual ~Voxel() {}
    Voxel(const Voxel&) = delete;
    Voxel(Voxel&&) = delete;
    Voxel& operator=(const Voxel&) = delete;
    Voxel& operator=(Voxel&&) = delete;

    glm::vec3 get_voxel_dimension() const {
        return dimension;
    }

    size_t get_voxel_size() const {
        return voxel_size;
    }

    virtual void bind(uint32_t p_index) const = 0;
    virtual void unbind(uint32_t p_index) const = 0;
    virtual void set_uniform(const Shader* p_shader, glm::vec3 p_voxels_lbb) const = 0;

protected:
    glm::ivec3 dimension;
    float voxel_size;

};

#endif
