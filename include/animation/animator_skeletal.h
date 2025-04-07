#ifndef __ANIMATOR_SKELETAL_H__
#define __ANIMATOR_SKELETAL_H__
#include "animation/skeletal_animation.h"
#include "data/skeleton.h"
#include "data/skeleton_model.h"
#include <glm/glm.hpp>

class AnimatorSkeletal {
public:

    AnimatorSkeletal(std::vector<SkeletalAnimation>&& p_animations);
    virtual ~AnimatorSkeletal() {
        animations.clear();
    }

    void update(double p_delta_t);
    void apply_bones(Skeleton* p_skeleton);
    void apply_morphs(SkeletonModel* p_model);

    void add_animation(const SkeletalAnimation& p_animation) {
        animations.push_back(p_animation);
    }

    void add_animation(SkeletalAnimation&& p_animation) noexcept {
        animations.push_back(std::move(p_animation));
    }

    void play_animation(const std::string& p_name);

    void play_animation(int p_index);

    void set_animation_playspeed(double p_playspeed) {
        playspeed = p_playspeed;
    }

    double get_animation_playspeed() const {
        return playspeed;
    }

    const std::vector<SkeletalAnimation>& get_animations() const {
        return animations;
    }

    double get_current_time() const {
        return time;
    }

    void set_current_time(double p_time) {
        time = fmod(p_time, animations[current_animation_index].get_duration());
    }

    std::vector<std::string> get_animations_names() const {
        std::vector<std::string> anim_names;
        anim_names.reserve(animations.size());
        for (auto& anim : animations) {
            anim_names.push_back(anim.get_name());
        }
        return anim_names;
    }

    int get_current_animation_index() const {
        return current_animation_index;
    }
    
private:

    double time;
    double playspeed = 1.0;
    std::vector<SkeletalAnimation> animations;
    int current_animation_index;

    void update_bones(const Skeleton::BoneInfo* p_current_node,
                      std::vector<BoneAnimation>& p_bone_animations,
                      const glm::mat4& p_parent_transform,
                      const std::vector<Skeleton::BoneInfo>& p_bone_infos,
                      Skeleton* p_skeleton);
    void calculate_bone_transforms(SkeletalAnimation& p_current_animation, Skeleton* p_skeleton);
};

#endif
