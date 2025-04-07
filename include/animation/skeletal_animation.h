#ifndef __SKELETAL_ANIMATION_H__
#define __SKELETAL_ANIMATION_H__
#include "animation/mesh_morph_animation.h"
#include "bone_animation.h"

class SkeletalAnimation {
public:
    SkeletalAnimation(const std::string& p_anim_name, std::vector<BoneAnimation>&& p_bone_animations,
                      std::vector<SingleMeshAnimation>&& p_morph_animations, double p_duration, double p_ticks_per_second);
    virtual ~SkeletalAnimation();

    const std::string& get_name() const { return anim_name; }
    
    const std::vector<BoneAnimation>& get_bone_animations() const {
        return bone_animations;
    }

    const std::vector<SingleMeshAnimation>& get_morph_animations() const {
        return morph_animations;
    }

    std::vector<BoneAnimation>& get_bone_animations() {
        return bone_animations;
    }

    std::vector<SingleMeshAnimation>& get_morph_animations() {
        return morph_animations;
    }

    double get_duration() const {
        return duration;
    }

    double get_ticks_per_second() {
        return ticks_per_second;
    }
private:
    std::string anim_name;
    std::vector<BoneAnimation> bone_animations;
    std::vector<SingleMeshAnimation> morph_animations;
    double duration;
    double ticks_per_second;
};

#endif
