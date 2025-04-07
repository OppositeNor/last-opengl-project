#include "animation/bone_animation.h"

BoneAnimation::BoneAnimation(const std::string& p_bone_name, int p_bone_id,
                  std::vector<KeyPosition>&& p_positions,
                  std::vector<KeyRotation>&& p_rotations,
                  std::vector<KeyScale>&& p_scales) noexcept
    : bone_name(p_bone_name), bone_id(p_bone_id),
    positions(std::move(p_positions)),
    rotations(std::move(p_rotations)),
    scales(std::move(p_scales)) {}

void BoneAnimation::update(double p_time, const glm::mat4& p_parent_global_transform) {
    glm::mat4 translation = interpolate_position(p_time);
    glm::mat4 rotation = interpolate_rotation(p_time);
    glm::mat4 scale = interpolate_scale(p_time);
    local_transform = translation * rotation * scale;
    global_transform = p_parent_global_transform * local_transform;
}

glm::mat4 BoneAnimation::interpolate_position(double p_time) {
    if (positions.size() == 0) {
        return glm::mat4(1.0);
    }
    if (positions.size() == 1) {
        return glm::translate(glm::mat4(1.0f), positions[0].position);
    }

    int p0 = get_position_index(p_time);
    if ((std::size_t)p0 == positions.size() - 1) {
        return glm::translate(glm::mat4(1.0f), positions[p0].position);
    }
    int p1 = (p0 + 1)/*  % positions.size() */;
    float scale_factor = get_scale_factor(positions[p0].time_stamp, positions[p1].time_stamp, p_time);
    return glm::translate(glm::mat4(1.0f), glm::mix(positions[p0].position, positions[p1].position, scale_factor));
}

int BoneAnimation::get_position_index(double p_time) {
    int size = positions.size() - 1;
    for (int i = 0; i < size; ++i) {
        if (p_time < positions[i + 1].time_stamp) {
            return i;
        }
    }
    return size;
}

glm::mat4 BoneAnimation::interpolate_rotation(double p_time) {
    if (rotations.size() == 0) {
        return glm::mat4(1.0);
    }
    if (rotations.size() == 1) {
        return glm::mat4_cast(glm::normalize(rotations[0].orientation));
    }

    int p0 = get_rotation_index(p_time);
    if ((std::size_t)p0 == rotations.size() - 1) {
        return glm::mat4_cast(glm::normalize(rotations[p0].orientation));
    }
    int p1 = (p0 + 1)/*  % rotations.size() */;
    float scale_factor = get_scale_factor(rotations[p0].time_stamp, rotations[p1].time_stamp, p_time);
    return glm::mat4_cast(glm::normalize(glm::slerp(rotations[p0].orientation, rotations[p1].orientation, scale_factor)));
}

int BoneAnimation::get_rotation_index(double p_time) {
    int size = rotations.size() - 1;
    for (int i = 0; i < size; ++i) {
        if (p_time < rotations[i + 1].time_stamp) {
            return i;
        }
    }
    return size;
}

glm::mat4 BoneAnimation::interpolate_scale(double p_time) {
    if (scales.size() == 0) {
        return glm::mat4(1.0);
    }
    if (scales.size() == 1) {
        return glm::scale(glm::mat4(1.0f), scales[0].scale);
    }
    int p0 = get_scale_index(p_time);
    if ((std::size_t)p0 == scales.size() - 1) {
        return glm::scale(glm::mat4(1.0f), scales[p0].scale);
    }
    int p1 = (p0 + 1)/*  % scales.size() */;
    float scale_factor = get_scale_factor(scales[p0].time_stamp, scales[p1].time_stamp, p_time);
    return glm::scale(glm::mat4(1.0f), glm::mix(scales[p0].scale, scales[p1].scale, scale_factor));
}

int BoneAnimation::get_scale_index(double p_time) {
    int size = rotations.size() - 1;
    for (int i = 0; i < size; ++i) {
        if (p_time < rotations[i + 1].time_stamp) {
            return i;
        }
    }
    return size;
}

float BoneAnimation::get_scale_factor(double p_p0_time_stamp, double p_p1_time_stamp, double p_time) {
    return (p_time - p_p0_time_stamp) / (p_p1_time_stamp - p_p0_time_stamp);
}
