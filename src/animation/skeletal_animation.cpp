#include "animation/skeletal_animation.h"

SkeletalAnimation::SkeletalAnimation(const std::string& p_anim_name, std::vector<BoneAnimation>&& p_bone_animations,
                                     std::vector<SingleMeshAnimation>&& p_morph_animations, double p_duration, double p_ticks_per_second)
    : anim_name(p_anim_name), bone_animations(std::move(p_bone_animations)), morph_animations(p_morph_animations),
    duration(p_duration), ticks_per_second(p_ticks_per_second) {
}

SkeletalAnimation::~SkeletalAnimation() {
}

