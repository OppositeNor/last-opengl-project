#ifndef __SKELETON_MODEL_H__
#define __SKELETON_MODEL_H__
#include "data/skeleton.h"
#include "material/material.hpp"
#include "shader/shader_compute.h"
#include "texture/texture_2d_frgba.h"

class SkeletonModel {
public:
    struct Mesh;

    SkeletonModel(std::vector<Mesh*>&& p_meshes, Texture2DFRGBA*&& p_bones_offsets, unsigned int p_object_id) noexcept;

    virtual ~SkeletonModel() {
        clear();
    }

    constexpr static std::size_t MAX_BONE_INFLUENCE = 4;

    // clear all data
    void clear();

    const Material* get_material(int p_index) const {
        return meshes[p_index]->material;
    }

    // draw the mesh
    void draw_mesh(std::size_t p_mesh_index) const;

    // get the mesh count
    std::size_t get_mesh_count() const {
        return meshes.size();
    }

    // get the basic offset of mesh
    const Texture2DFRGBA* get_bones_offsets() const {
        return bones_offsets;
    }

    void update_mesh_anim(int p_mesh_index, int p_attach_index, float p_weight);

    struct Mesh {
        Mesh() {}
        virtual ~Mesh();
        // Because of alignment of vec4 in GLSL, all vec3 here should be written as vec4
        #pragma pack(push, 1)
        struct Vertex {
            glm::vec4 position;
            glm::vec4 normal;
            glm::vec4 texture_uv;
            glm::vec4 tangent;
            glm::vec4 bitangent;
            glm::ivec4 bone_ids;
            glm::vec4 bone_weights;
            Vertex()
            : position(0, 0, 0, 0), normal(0, 0, 0, 0), texture_uv(0, 0, 0, 0), tangent(0, 0, 0, 0) {}
        };
        #pragma pack(pop)
        Material* material = nullptr;
        std::vector<unsigned int> indices;
        virtual void on_draw() const = 0;
        mutable unsigned int vao=0, vbo=0, ebo=0;
        virtual void init_draw_data() const = 0;
        void init_offsets() const;
        virtual void update(std::shared_ptr<const Skeleton> p_skeleton, const Texture2DFRGBA* p_bones_offsets) const = 0;
        virtual unsigned int get_vertices_count() const = 0;
        virtual void reset() = 0;
    };

    struct StaticMesh : public Mesh {
        virtual ~StaticMesh() override;
        std::vector<Vertex> vertices;
        mutable std::shared_ptr<ShaderCompute> skeleton_compute_shader;
        virtual void on_draw() const override;
        virtual void init_draw_data() const override;
        virtual void update(std::shared_ptr<const Skeleton> p_skeleton, const Texture2DFRGBA* p_bones_offsets) const override;
        virtual unsigned int get_vertices_count() const override { return vertices.size(); }
        virtual void reset() override {}
        mutable unsigned int default_vertices_ssbo = 0;
        mutable unsigned int debug_ssbo = 0;
        void get_debug_output(std::vector<int>& p_output) const;
    };

    // Because of alignment of vec4 in GLSL, all vectors here should be written as vec4
    #pragma pack(push, 1)
    struct MorphKeyData {
        glm::vec4 position;
        glm::vec4 normal;
        glm::vec4 texture_uv;
        glm::vec4 tangent;
        glm::vec4 bitangent;
        MorphKeyData();
    };
    #pragma pack(pop)

    struct MorphKey {
        std::vector<MorphKeyData> vertices;
        float weight = 0.0;
    };

    struct MorphAnimMesh : public Mesh {
        constexpr static std::size_t MORPH_NUM_MAX = 126;
        MorphAnimMesh();
        mutable std::shared_ptr<ShaderCompute> skeleton_compute_shader;
        mutable std::shared_ptr<ShaderCompute> morph_compute_shader;
        virtual ~MorphAnimMesh() override;
        std::vector<Mesh::Vertex> default_vertices;
        std::vector<MorphKey> morph_anims_keys;
        mutable unsigned int debug_ssbo = 0;
        // TODO: Maybe just use ssbo as vbo?
        mutable unsigned int default_vertices_ssbo = 0;
        mutable unsigned int morph_keys_data_ssbo = 0;
        virtual void on_draw() const override;
        virtual void init_draw_data() const override;
        virtual void update(std::shared_ptr<const Skeleton> p_skeleton, const Texture2DFRGBA* p_bones_offsets) const override;
        virtual unsigned int get_vertices_count() const override { return default_vertices.size(); }
        virtual void reset() override;
        void compute_morph() const;
        void get_debug_output(std::vector<int>& p_output) const;
    };

    void update(std::shared_ptr<const Skeleton> p_skeleton) const;

    void reset_meshes();

    int get_object_id() const {
        return object_id;
    }
private:

    constexpr static unsigned int POSITION_OFFSET = offsetof(Mesh::Vertex, position);
    constexpr static unsigned int NORMAL_OFFSET = offsetof(Mesh::Vertex, normal);
    constexpr static unsigned int TEXTURE_UV_OFFSET = offsetof(Mesh::Vertex, texture_uv);
    constexpr static unsigned int TANGENT_OFFSET = offsetof(Mesh::Vertex, tangent);
    constexpr static unsigned int BITANGENT_OFFSET = offsetof(Mesh::Vertex, bitangent);
    constexpr static unsigned int BONE_ID_OFFSET = offsetof(Mesh::Vertex, bone_ids);
    constexpr static unsigned int BONE_WEIGHT_OFFSET = offsetof(Mesh::Vertex, bone_weights);

    std::vector<Mesh*> meshes;

    Texture2DFRGBA* bones_offsets;


    int object_id;

    void init_mesh_vao(const Mesh* mesh) const;
};

#endif
