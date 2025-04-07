#ifndef __SKELETON_H__
#define __SKELETON_H__
#include "shader/shader.h"
#include "texture/texture_2d_frgba.h"
#include <string>
#include <map>
#include <memory>
#include <vector>

class Skeleton {
public:

    static constexpr glm::vec3 SKELETON_COLOR = glm::vec3(0.0, 0.0, 1.0);

    struct BoneInfo {
        int id = -1;
        std::string bone_name = "";
        std::vector<int> children_ids = std::vector<int>();
        glm::mat4 node_transform = glm::mat4(1.0);
    };

    Skeleton(int p_root_bone_index, const std::vector<BoneInfo>& p_bone_infos, std::size_t p_anim_max_bones) noexcept;
    virtual ~Skeleton();

    void reset_pose();

    std::shared_ptr<const std::map<std::string, int>> get_bone_map() const {
        return bone_map;
    }
    
    int get_root_bone_id() const {
        return root_bone_id;
    }

    void set_bone_transform(int p_bone_id, const glm::mat4& p_transform) {
        if ((std::size_t)p_bone_id < anim_max_bones) {
            final_transforms->set_texture_row(p_bone_id, &(p_transform[0][0]));
        }
    }

    const Texture2DFRGBA* get_bones_final_transforms() const {
        return final_transforms;
    }

    const std::vector<BoneInfo>& get_bone_infos() const {
        return bone_infos;
    }

    void render(const glm::mat4& p_model, const RenderData& p_render_data) const;

    void highlight_bone(int p_id) const {
        highlighted_bone_id = p_id;
    }

    int get_highlighting_bone() const {
        return highlighted_bone_id;
    }

private:
    std::shared_ptr<std::map<std::string, int>> bone_map;
    int root_bone_id = -1;
    std::vector<BoneInfo> bone_infos;
    Texture2DFRGBA* final_transforms = nullptr;
    void reset_bone(const BoneInfo* p_bone_info, const glm::mat4& p_current_transform);
    std::size_t anim_max_bones;

    struct SkeletonDrawDataVertex {
        glm::vec3 color;
        int bone_id;
    };

    mutable std::shared_ptr<Shader> draw_shader = nullptr;
    mutable std::vector<SkeletonDrawDataVertex> skeleton_draw_data_vertices;
    mutable unsigned int vao = 0, vbo;

    unsigned int get_vao() const;
    void init_draw_data() const;
    int get_bones_depth(int p_curr_bone_id) const;
    void init_vertices(int p_bone_info_id, int p_curr_depth, int p_bones_depth) const;

    mutable int highlighted_bone_id = -1;
};

#endif
