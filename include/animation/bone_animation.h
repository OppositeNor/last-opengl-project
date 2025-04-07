#ifndef __BONE_ANIMATION_H__
#define __BONE_ANIMATION_H__

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

struct KeyPosition {
    glm::vec3 position;
    double time_stamp;
};
struct KeyRotation {
    glm::quat orientation;
    double time_stamp;
};
struct KeyScale {
    glm::vec3 scale;
    double time_stamp;
};

class BoneAnimation {
public:
    BoneAnimation()
        : BoneAnimation("", -1, std::vector<KeyPosition>(), std::vector<KeyRotation>(), std::vector<KeyScale>()) {}
    BoneAnimation(const std::string& p_bone_name, int p_bone_id,
                  std::vector<KeyPosition>&& p_positions,
                  std::vector<KeyRotation>&& p_rotations,
                  std::vector<KeyScale>&& p_scales) noexcept;

    void update(double p_time, const glm::mat4& p_parent_global_transform);

    const std::string& get_bone_name() const { return bone_name; }

    int get_id() const { return bone_id; }

    const glm::mat4& get_global_transform() { return global_transform; }
private:
    std::string bone_name;
    int bone_id;
    std::vector<KeyPosition> positions;
    std::vector<KeyRotation> rotations;
    std::vector<KeyScale> scales;
    glm::mat4 global_transform;
    glm::mat4 local_transform;

    glm::mat4 interpolate_position(double p_time);
    int get_position_index(double p_time);
    glm::mat4 interpolate_rotation(double p_time);
    int get_rotation_index(double p_time);
    glm::mat4 interpolate_scale(double p_time);
    int get_scale_index(double p_time);

    float get_scale_factor(double p_p0_time_stamp, double p_p1_time_stamp, double p_time);
};


#endif
