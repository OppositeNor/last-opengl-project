#ifndef __OBJ_H__
#define __OBJ_H__
#include "material/material.hpp"
#include "resource/resource_base.h"
#include "utils.hpp"
#include <vector>

template<>
struct get_res_type<class OBJModel> {
    constexpr static const char* VALUE = "obj_model";
};

// Class for loading obj files
class OBJModel : public ResourceBase {
public:
    constexpr static size_t VERTEX_SIZE = 11;
    constexpr static size_t VERTEX_POSITION_OFFSET = 0;
    constexpr static size_t VERTEX_NORMAL_OFFSET = 3;
    constexpr static size_t VERTEX_TEXCOORD_OFFSET = 6;
    constexpr static size_t VERTEX_TANGENT_OFFSET = 8;
    
    OBJModel(unsigned int p_object_id, const std::string& p_path);
    OBJModel(const OBJModel&) = delete;
    virtual ~OBJModel();

    virtual void load() override;

    // Get the vertices from the obj file being parsed.
    const float* get_vertices() const {
        return reinterpret_cast<const float*>(m_vertices.data());
    }

    // Get the number of vertices in the obj file.
    size_t get_vertices_count() const {
        return m_vertices.size() * VERTEX_SIZE;
    }

    // Get the indices from the obj file being parsed.
    const unsigned int* get_indices() const {
        return m_indices.data();
    }

    // Get the number of indices in the obj file.
    size_t get_indices_count() const {
        return m_indices.size();
    }

    // Get the material from the obj file being parsed.
    const std::shared_ptr<Material> get_material() const {
        return material;
    }

    void set_material(std::shared_ptr<Material> p_material) {
        material = p_material;
    }

    void set_material(Material*&& p_material) {
        material = std::shared_ptr<Material>(std::move(p_material));
    }

    int get_object_id() const {
        return object_id;
    }

    // Draw the obj file
    void draw() const;

    // Update the vertex data
    void update_vertex_data();
private:

    std::string path;

    // Parse the obj file and store the vertices, and normals
    void parse(const std::string& p_path);
    std::vector<Tuple<float, VERTEX_SIZE>> m_vertices;
    std::vector<unsigned int> m_indices;
    std::shared_ptr<Material> material;
    mutable unsigned int vao, vbo, ebo;
    int object_id;

    void init_vertex_data() const;
};

#endif
