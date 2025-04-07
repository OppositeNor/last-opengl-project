#include "data/skeleton_model.h"
#include "graphics.h"
#include "resource/global_resource.h"
#include "utils.hpp"
#include <cstring>
#include <memory>
#include <string>

SkeletonModel::SkeletonModel(std::vector<Mesh*>&& p_meshes, Texture2DFRGBA*&& p_bones_offsets, unsigned int p_object_id) noexcept
: meshes(std::move(p_meshes)), bones_offsets(REQUIRE_NON_NULL(p_bones_offsets)), object_id(p_object_id) {
    p_bones_offsets = nullptr;
}


void SkeletonModel::clear() {
    for (auto mesh : meshes) {
        delete mesh;
    }
    meshes.clear();
    delete bones_offsets;
}

SkeletonModel::Mesh::~Mesh() {
    if (vao != 0) {
        GL_CALL(glDeleteVertexArrays, 1, &vao);
    }
    if (vbo != 0) {
        GL_CALL(glDeleteBuffers, 1, &vbo);
    }
    if (ebo != 0) {
        GL_CALL(glDeleteBuffers, 1, &ebo);
    }
    delete material;
}

SkeletonModel::MorphKeyData::MorphKeyData()
    : position(0, 0, 0, 0), normal(0, 0, 1, 0), texture_uv(0, 0, 0, 0), tangent(1, 0, 0, 0) {
}


void SkeletonModel::draw_mesh(std::size_t p_mesh_index) const {
    if (meshes[p_mesh_index]->vao == 0) {
        init_mesh_vao(meshes[p_mesh_index]);
    }
    meshes[p_mesh_index]->on_draw();
}

void SkeletonModel::update_mesh_anim(int p_mesh_index, int p_attach_index, float p_weight) {
    MorphAnimMesh* mesh = REQUIRE_NON_NULL(dynamic_cast<MorphAnimMesh*>(meshes[p_mesh_index]));
    mesh->morph_anims_keys[p_attach_index].weight = p_weight;
}

SkeletonModel::MorphAnimMesh::MorphAnimMesh() {
}

SkeletonModel::MorphAnimMesh::~MorphAnimMesh() {
    if (debug_ssbo != 0) {
        GL_CALL(glDeleteBuffers, 1, &debug_ssbo);
    }
    if (default_vertices_ssbo != 0) {
        GL_CALL(glDeleteBuffers, 1, &default_vertices_ssbo);
    }
    if (morph_keys_data_ssbo != 0) {
        GL_CALL(glDeleteBuffers, 1, &morph_keys_data_ssbo);
    }
}

SkeletonModel::StaticMesh::~StaticMesh() {
    if (debug_ssbo != 0) {
        GL_CALL(glDeleteBuffers, 1, &debug_ssbo);
    }
}

void SkeletonModel::update(std::shared_ptr<const Skeleton> p_skeleton) const {
    for (auto* mesh : meshes) {
        mesh->update(p_skeleton, bones_offsets);
    }
}

void SkeletonModel::reset_meshes() {
    for (auto* mesh : meshes) {
        mesh->reset();
    }
}

void SkeletonModel::Mesh::init_offsets() const {
    GL_CALL(glEnableVertexAttribArray, VERTEX_LOCATION_POS);
    GL_CALL(glVertexAttribPointer, VERTEX_LOCATION_POS, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex),
            (void*)(POSITION_OFFSET));
    GL_CALL(glEnableVertexAttribArray, VERTEX_LOCATION_NORMAL);
    GL_CALL(glVertexAttribPointer, VERTEX_LOCATION_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex),
            (void*)(NORMAL_OFFSET));
    GL_CALL(glEnableVertexAttribArray, VERTEX_LOCATION_UV);
    GL_CALL(glVertexAttribPointer, VERTEX_LOCATION_UV, 2, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex),
            (void*)(TEXTURE_UV_OFFSET));
    GL_CALL(glEnableVertexAttribArray, VERTEX_LOCATION_TANGENT);
    GL_CALL(glVertexAttribPointer, VERTEX_LOCATION_TANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex),
            (void*)(TANGENT_OFFSET));
    GL_CALL(glEnableVertexAttribArray, VERTEX_LOCATION_BITANGENT);
    GL_CALL(glVertexAttribPointer, VERTEX_LOCATION_BITANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex),
            (void*)(BITANGENT_OFFSET));
    GL_CALL(glEnableVertexAttribArray, VERTEX_LOCATION_BONE_ID);
    GL_CALL(glVertexAttribIPointer, VERTEX_LOCATION_BONE_ID, MAX_BONE_INFLUENCE, GL_INT, sizeof(Mesh::Vertex),
            (void*)(BONE_ID_OFFSET));
    GL_CALL(glEnableVertexAttribArray, VERTEX_LOCATION_BONE_WEIGHT);
    GL_CALL(glVertexAttribPointer, VERTEX_LOCATION_BONE_WEIGHT, MAX_BONE_INFLUENCE, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex),
            (void*)(BONE_WEIGHT_OFFSET));
}

void SkeletonModel::StaticMesh::update(std::shared_ptr<const Skeleton> p_skeleton, const Texture2DFRGBA* p_bones_offsets) const {
    if (vao == 0) {
        return;
    }
    skeleton_compute_shader->use();
    GL_CALL(glBindBufferBase, GL_SHADER_STORAGE_BUFFER, 0, debug_ssbo);
    GL_CALL(glBindBufferBase, GL_SHADER_STORAGE_BUFFER, 1, default_vertices_ssbo);
    GL_CALL(glBindBufferBase, GL_SHADER_STORAGE_BUFFER, 2, vbo);
    skeleton_compute_shader->set_uniform("bones_transforms", p_skeleton->get_bones_final_transforms());
    skeleton_compute_shader->set_uniform("bones_offsets", p_bones_offsets);
    skeleton_compute_shader->set_uniform("vert_count", static_cast<int>(vertices.size()), true);
    skeleton_compute_shader->compute(vertices.size() / 64 + 1, 1, 1);
    skeleton_compute_shader->set_shader_storage_barrier();
    skeleton_compute_shader->set_va_barrier();
    GL_CALL(glBindBufferBase, GL_SHADER_STORAGE_BUFFER, 0, 0);
    GL_CALL(glBindBufferBase, GL_SHADER_STORAGE_BUFFER, 1, 0);
    GL_CALL(glBindBufferBase, GL_SHADER_STORAGE_BUFFER, 2, 0);
}

void SkeletonModel::MorphAnimMesh::update(std::shared_ptr<const Skeleton> p_skeleton, const Texture2DFRGBA* p_bones_offsets) const {
    if (vao == 0) {
        return;
    }
    morph_compute_shader->use();
    GL_CALL(glBindBufferBase, GL_SHADER_STORAGE_BUFFER, 0, debug_ssbo);
    GL_CALL(glBindBufferBase, GL_SHADER_STORAGE_BUFFER, 1, default_vertices_ssbo);
    GL_CALL(glBindBufferBase, GL_SHADER_STORAGE_BUFFER, 2, morph_keys_data_ssbo);
    GL_CALL(glBindBufferBase, GL_SHADER_STORAGE_BUFFER, 3, vbo);
    std::vector<float> weights(morph_anims_keys.size());
    for (std::size_t i = 0; i < morph_anims_keys.size(); ++i) {
        weights[i] = morph_anims_keys[i].weight;
    }
    morph_compute_shader->set_uniform("morph_weights", weights.data(), weights.size(), true);
    morph_compute_shader->set_uniform("morph_counts", static_cast<int>(morph_anims_keys.size()), true);
    morph_compute_shader->set_uniform("vert_count", static_cast<int>(default_vertices.size()), true);
    morph_compute_shader->compute(default_vertices.size() / 64 + 1, 1, 1);
    morph_compute_shader->set_shader_storage_barrier();
    morph_compute_shader->set_va_barrier();
    GL_CALL(glBindBufferBase, GL_SHADER_STORAGE_BUFFER, 0, 0);
    GL_CALL(glBindBufferBase, GL_SHADER_STORAGE_BUFFER, 1, 0);
    GL_CALL(glBindBufferBase, GL_SHADER_STORAGE_BUFFER, 2, 0);
    GL_CALL(glBindBufferBase, GL_SHADER_STORAGE_BUFFER, 3, 0);
    morph_compute_shader->unuse();

    skeleton_compute_shader->use();
    GL_CALL(glBindBufferBase, GL_SHADER_STORAGE_BUFFER, 0, debug_ssbo);
    GL_CALL(glBindBufferBase, GL_SHADER_STORAGE_BUFFER, 1, vbo);
    GL_CALL(glBindBufferBase, GL_SHADER_STORAGE_BUFFER, 2, vbo);
    skeleton_compute_shader->set_uniform("bones_transforms", p_skeleton->get_bones_final_transforms());
    skeleton_compute_shader->set_uniform("bones_offsets", p_bones_offsets);
    skeleton_compute_shader->set_uniform("vert_count", static_cast<int>(default_vertices.size()), true);
    skeleton_compute_shader->compute(default_vertices.size() / 64 + 1, 1, 1);
    skeleton_compute_shader->set_shader_storage_barrier();
    skeleton_compute_shader->set_va_barrier();
    GL_CALL(glBindBufferBase, GL_SHADER_STORAGE_BUFFER, 0, 0);
    GL_CALL(glBindBufferBase, GL_SHADER_STORAGE_BUFFER, 1, 0);
    GL_CALL(glBindBufferBase, GL_SHADER_STORAGE_BUFFER, 2, 0);
    skeleton_compute_shader->unuse();
}

void SkeletonModel::StaticMesh::get_debug_output(std::vector<int>& p_output) const {
    GL_CALL(glBindBuffer, GL_SHADER_STORAGE_BUFFER, debug_ssbo);
    int* buff = (int*)GL_CALL(glMapBuffer, GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    p_output.resize(vertices.size());
    memcpy(p_output.data(), buff, vertices.size() * sizeof(int));
    GL_CALL(glUnmapBuffer, GL_SHADER_STORAGE_BUFFER);
}


void SkeletonModel::MorphAnimMesh::get_debug_output(std::vector<int>& p_output) const {
    GL_CALL(glBindBuffer, GL_SHADER_STORAGE_BUFFER, debug_ssbo);
    int* buff = (int*)GL_CALL(glMapBuffer, GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    p_output.resize(default_vertices.size());
    memcpy(p_output.data(), buff, default_vertices.size() * sizeof(int));
    GL_CALL(glUnmapBuffer, GL_SHADER_STORAGE_BUFFER);
}

void SkeletonModel::MorphAnimMesh::reset() {
    if (vao == 0) {
        return;
    }
    for (auto& morph_anim : morph_anims_keys) {
        morph_anim.weight = 0.0;
    }
}


void SkeletonModel::StaticMesh::init_draw_data() const {
    skeleton_compute_shader = GlobalResource::get_singleton()->get_resource<ShaderCompute>("shader_skeletal_mesh_skeleton_compute");
    GL_CALL(glGenBuffers, 1, &debug_ssbo);
    GL_CALL(glBindBuffer, GL_SHADER_STORAGE_BUFFER, debug_ssbo);
    std::unique_ptr<int[]> default_debug_values(new int[vertices.size()]);
    memset(default_debug_values.get(), 0, vertices.size() * sizeof(int));
    GL_CALL(glBufferData, GL_SHADER_STORAGE_BUFFER, vertices.size() * sizeof(int), default_debug_values.get(), GL_DYNAMIC_READ);
    GL_CALL(glGenBuffers, 1, &default_vertices_ssbo);
    GL_CALL(glBindBuffer, GL_SHADER_STORAGE_BUFFER, default_vertices_ssbo);
    GL_CALL(glBufferData, GL_SHADER_STORAGE_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_READ);
    GL_CALL(glBindBuffer, GL_SHADER_STORAGE_BUFFER, 0);
    GL_CALL(glGenVertexArrays, 1, &vao);
    GL_CALL(glGenBuffers, 1, &vbo);
    GL_CALL(glGenBuffers, 1, &ebo);
    GL_CALL(glBindVertexArray, vao);
    GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vbo);
    GL_CALL(glBufferData, GL_ARRAY_BUFFER, vertices.size() * sizeof(Mesh::Vertex), vertices.data(), GL_DYNAMIC_DRAW);
    GL_CALL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, ebo);
    GL_CALL(glBufferData, GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
            indices.data(), GL_STATIC_DRAW);
    init_offsets();
    GL_CALL(glBindVertexArray, 0);
}

void SkeletonModel::StaticMesh::on_draw() const {
    GL_CALL(glBindVertexArray, vao);
    GL_CALL(glDrawElements, GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    GL_CALL(glBindVertexArray, 0);
}

void SkeletonModel::MorphAnimMesh::on_draw() const {
    GL_CALL(glBindVertexArray, vao);
    GL_CALL(glDrawElements, GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    GL_CALL(glBindVertexArray, 0);
}

void SkeletonModel::MorphAnimMesh::init_draw_data() const {
    morph_compute_shader = GlobalResource::get_singleton()->get_resource<ShaderCompute>("shader_skeletal_mesh_morph_compute");
    skeleton_compute_shader = GlobalResource::get_singleton()->get_resource<ShaderCompute>("shader_skeletal_mesh_skeleton_compute");

    GL_CALL(glGenVertexArrays, 1, &vao);
    GL_CALL(glGenBuffers, 1, &vbo);
    GL_CALL(glGenBuffers, 1, &ebo);
    GL_CALL(glGenBuffers, 1, &debug_ssbo);
    GL_CALL(glBindBuffer, GL_SHADER_STORAGE_BUFFER, debug_ssbo);
    std::unique_ptr<int[]> default_debug_values(new int[default_vertices.size()]);
    memset(default_debug_values.get(), 0, default_vertices.size() * sizeof(int));
    GL_CALL(glBufferData, GL_SHADER_STORAGE_BUFFER, default_vertices.size() * sizeof(int), default_debug_values.get(), GL_DYNAMIC_READ);
    GL_CALL(glBindVertexArray, vao);
    GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vbo);
    GL_CALL(glBufferData, GL_ARRAY_BUFFER, default_vertices.size() * sizeof(Mesh::Vertex), default_vertices.data(), GL_DYNAMIC_DRAW);
    GL_CALL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, ebo);
    GL_CALL(glBufferData, GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
            indices.data(), GL_STATIC_DRAW);

    {
        std::size_t chunk_size = default_vertices.size() * sizeof(Mesh::Vertex);
        GL_CALL(glGenBuffers, 1, &default_vertices_ssbo);
        GL_CALL(glBindBuffer, GL_SHADER_STORAGE_BUFFER, default_vertices_ssbo);
        GL_CALL(glBufferData, GL_SHADER_STORAGE_BUFFER, chunk_size, default_vertices.data(), GL_DYNAMIC_READ);
        GL_CALL(glBindBuffer, GL_SHADER_STORAGE_BUFFER, 0);
    }

    {
        std::size_t chunk_size = default_vertices.size() * sizeof(MorphKeyData);
        GL_CALL(glGenBuffers, 1, &morph_keys_data_ssbo);
        GL_CALL(glBindBuffer, GL_SHADER_STORAGE_BUFFER, morph_keys_data_ssbo);
        GL_CALL(glBufferData, GL_SHADER_STORAGE_BUFFER, chunk_size * morph_anims_keys.size(), nullptr, GL_STATIC_READ);
        for (std::size_t i = 0; i < morph_anims_keys.size(); ++i) {
            auto& morph_anim = morph_anims_keys[i];
            GL_CALL(glBufferSubData, GL_SHADER_STORAGE_BUFFER, chunk_size * i, chunk_size, morph_anim.vertices.data());
        }
        GL_CALL(glBindBuffer, GL_SHADER_STORAGE_BUFFER, 0);
    }

    init_offsets();
    GL_CALL(glBindVertexArray, 0);
}

void SkeletonModel::init_mesh_vao(const Mesh* mesh) const {
    if (mesh->vao != 0) {
        return;
    }
    mesh->init_draw_data();
}
