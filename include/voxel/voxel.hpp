#ifndef __VOXEL_H__
#define __VOXEL_H__

#include <cstddef>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>

class Voxel {
public:
    Voxel(glm::vec3 p_dimension, size_t p_voxel_size)
        : dimension(p_dimension), voxel_size(p_voxel_size) {}
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

protected:
    glm::vec3 dimension;
    size_t voxel_size;

};

#endif
