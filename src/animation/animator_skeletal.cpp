#include "animation/animator_skeletal.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <cmath>
#include <ctime>

AnimatorSkeletal::AnimatorSkeletal(std::vector<SkeletalAnimation>&& p_animations)
    : animations(std::move(p_animations)), current_animation_index(-1) {
}

void AnimatorSkeletal::update(double p_delta_t) {
    if (current_animation_index < 0) {
        return;
    }
    auto& current_animation = animations[current_animation_index];
    time += p_delta_t * current_animation.get_ticks_per_second() * playspeed;
    time = fmod(time, current_animation.get_duration());
}

void AnimatorSkeletal::apply_bones(Skeleton* p_skeleton) {
    if (current_animation_index < 0) {
        return;
    }
    auto& current_animation = animations[current_animation_index];
    calculate_bone_transforms(current_animation, p_skeleton);
}

void AnimatorSkeletal::apply_morphs(SkeletonModel* p_model) {
    if (current_animation_index < 0) {
        return;
    }
    auto& morphs = animations[current_animation_index].get_morph_animations();
    for (auto& morph_anim : morphs) {
        MeshAnimationValue interpolated_morphs;
        morph_anim.interpolate_values(time, interpolated_morphs);
        for (unsigned int i = 0; i < interpolated_morphs.indices.size(); ++i) {
            for (auto mesh_index : interpolated_morphs.mesh_indices) {
                p_model->update_mesh_anim(mesh_index, interpolated_morphs.indices[i], interpolated_morphs.values[i]);
            }
        }
    }
}

void AnimatorSkeletal::update_bones(const Skeleton::BoneInfo* p_current_node,
                                    std::vector<BoneAnimation>& p_bone_animations,
                                    const glm::mat4& p_global_transform,
                                    const std::vector<Skeleton::BoneInfo>& p_bone_infos,
                                    Skeleton* p_skeleton) {
    glm::mat4 bone_global_transform;
    auto curr_node_id = p_current_node->id;
    if (curr_node_id >= (int)p_bone_animations.size() || p_bone_animations[curr_node_id].get_id() == -1) {
        bone_global_transform = p_global_transform * p_bone_infos[p_current_node->id].node_transform;
    }
    else {
        p_bone_animations[curr_node_id].update(time, p_global_transform);
        bone_global_transform = p_bone_animations[curr_node_id].get_global_transform();
    }
    p_skeleton->set_bone_transform(curr_node_id, bone_global_transform);
    for (auto child_id : p_current_node->children_ids) {
        auto* child = &p_bone_infos[child_id];
        update_bones(child, p_bone_animations, bone_global_transform, p_bone_infos, p_skeleton);
    }
}

void AnimatorSkeletal::calculate_bone_transforms(SkeletalAnimation& p_current_animation, Skeleton* p_skeleton) {
    auto& bone_animations = p_current_animation.get_bone_animations();
    int root_bone_id = p_skeleton->get_root_bone_id();
    update_bones(&(p_skeleton->get_bone_infos()[root_bone_id]), bone_animations, glm::mat4(1.0),
                 p_skeleton->get_bone_infos(), p_skeleton);
}

void AnimatorSkeletal::play_animation(const std::string& p_name) {
    int i = 0;
    for (auto& anim : animations) {
        if (anim.get_name() == p_name) {
            play_animation(i);
            return;
        }
        ++i;
    }
    std::cout << "Animation: " << p_name << " not found!";
}

void AnimatorSkeletal::play_animation(int p_index) {
    current_animation_index = p_index;
    time = 0;
}
