#include "data/skeleton.h"
#include "graphics.h"
#include "resource/global_resource.h"
#include "camera.h"
#include "shader/shader_pipeline.h"
#include "utils.hpp"

Skeleton::Skeleton(int p_root_bone_index, const std::vector<BoneInfo>& p_bone_infos, std::size_t p_anim_max_bones) noexcept 
        : bone_infos(p_bone_infos), anim_max_bones(p_anim_max_bones) {
    root_bone_id = p_root_bone_index;
    reset_pose();
}

Skeleton::~Skeleton() {
    if (final_transforms != nullptr) {
        delete final_transforms;
    }
    if (vao != 0) {
        GL_CALL(glDeleteVertexArrays, 1, &vao);
        GL_CALL(glDeleteBuffers, 1, &vbo);
    }
}

void Skeleton::reset_pose() {
    if (final_transforms == nullptr) {
        TextureConfigurations config;
        config.generate_mipmap = false;
        config.scale_nearest_min = true;
        config.scale_nearest_mag = true;
        final_transforms = new Texture2DFRGBA(config);
    }
    final_transforms->create_empty_texture(4, anim_max_bones);
    auto identity = glm::mat4(1.0);
    for (std::size_t i = 0; i < anim_max_bones; ++i) {
        final_transforms->set_texture_row(i, &(identity[0][0]));
    }
    reset_bone(&(bone_infos[root_bone_id]), glm::mat4(1.0));
}

void Skeleton::reset_bone(const BoneInfo* p_bone_info, const glm::mat4& p_current_transform) {
    auto global_transform = p_current_transform * p_bone_info->node_transform;
    set_bone_transform(p_bone_info->id, global_transform);
    for (auto& child_id : p_bone_info->children_ids) {
        reset_bone(&(bone_infos[child_id]), global_transform);
    }
}

void Skeleton::render(const glm::mat4& p_model, const RenderData& p_render_data) const {
    if (draw_shader == nullptr) {
        draw_shader = REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<ShaderPipeline>("shader_skeleton"));
    }
    draw_shader->reset();
    draw_shader->use();
    draw_shader->set_uniform("model", p_model, true);
    draw_shader->set_uniform("view", REQUIRE_NON_NULL(p_render_data.camera)->get_view_matrix(), true);
    draw_shader->set_uniform("proj", p_render_data.proj_mat, true);
    draw_shader->set_uniform("bones_transforms", final_transforms, true);
    draw_shader->set_uniform("highlighted_bone_id", highlighted_bone_id, true);
    GL_CALL(glLineWidth, 5);
    GL_CALL(glBindVertexArray, get_vao());
    GL_CALL(glDrawArrays, GL_LINES, 0, skeleton_draw_data_vertices.size());
    GL_CALL(glBindVertexArray, 0);
    draw_shader->unuse();
}

unsigned int Skeleton::get_vao() const {
    if (vao == 0) {
        init_draw_data();
    }
    return vao;
}

void Skeleton::init_draw_data() const {
    if (vao != 0) {
        GL_CALL(glDeleteVertexArrays, 1, &vao);
        GL_CALL(glDeleteBuffers, 1, &vbo);
        vao = 0;
        vbo = 0;
    }

    skeleton_draw_data_vertices.clear();
    skeleton_draw_data_vertices.reserve(2 * bone_infos.size());
    init_vertices(root_bone_id, 0, get_bones_depth(root_bone_id));

    GL_CALL(glGenVertexArrays, 1, &vao);
    GL_CALL(glBindVertexArray, vao);
    GL_CALL(glGenBuffers, 1, &vbo);
    GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vbo);
    GL_CALL(glBufferData, GL_ARRAY_BUFFER, sizeof(SkeletonDrawDataVertex) * skeleton_draw_data_vertices.size(),
            skeleton_draw_data_vertices.data(), GL_STATIC_DRAW);
    // color
    GL_CALL(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, sizeof(SkeletonDrawDataVertex),
            reinterpret_cast<void*>(offsetof(SkeletonDrawDataVertex, color)));
    GL_CALL(glEnableVertexAttribArray, 0);
    // bone_id
    GL_CALL(glVertexAttribIPointer, 1, 1, GL_INT, sizeof(SkeletonDrawDataVertex),
            reinterpret_cast<void*>(offsetof(SkeletonDrawDataVertex, bone_id)));
    GL_CALL(glEnableVertexAttribArray, 1);
    GL_CALL(glBindVertexArray, 0);
}

int Skeleton::get_bones_depth(int p_curr_bone_id) const {
    int max_depth = 0;
    for (int child_id : bone_infos[p_curr_bone_id].children_ids) {
        int child_depth = get_bones_depth(child_id);
        if (child_depth > max_depth) {
            max_depth = child_depth;
        }
    }
    if ((std::size_t)p_curr_bone_id < anim_max_bones) {
        ++max_depth;
    }
    return max_depth;
}

void Skeleton::init_vertices(int p_bone_info_id, int p_curr_depth, int p_bones_depth) const {
    SkeletonDrawDataVertex curr_vertex;
    curr_vertex.bone_id = p_bone_info_id;
    if ((std::size_t)curr_vertex.bone_id < anim_max_bones) {
        ++p_curr_depth;
    }
    glm::vec3 bone_color = SKELETON_COLOR * (float)p_curr_depth / (float)p_bones_depth;
    curr_vertex.color = bone_color;
    for (int child_id : bone_infos[p_bone_info_id].children_ids) {
        SkeletonDrawDataVertex child_vertex;
        child_vertex.color = bone_color;
        child_vertex.bone_id = child_id;
        if ((std::size_t)curr_vertex.bone_id < anim_max_bones
            && (std::size_t)child_id < anim_max_bones) {
            skeleton_draw_data_vertices.push_back(curr_vertex);
            skeleton_draw_data_vertices.push_back(child_vertex);
        }
        init_vertices(child_id, p_curr_depth, p_bones_depth);
    }
}
