#include "animation/mesh_morph_animation.h"

void SingleMeshAnimation::interpolate_values(double p_time, MeshAnimationValue& p_result) const {
    p_result.indices.reserve(attach_mesh_animations.size());
    p_result.values.reserve(attach_mesh_animations.size());
    p_result.mesh_indices = mesh_indices;
    for (auto& attach_mesh_animation : attach_mesh_animations) {
        p_result.indices.push_back(attach_mesh_animation.index);
        if (attach_mesh_animation.times.size() == 1) {
            p_result.values.push_back(attach_mesh_animation.values[0]);
            continue;
        }
        std::size_t p1;
        for (p1 = 0; p1 < attach_mesh_animation.times.size(); ++p1) {
            if (p_time < attach_mesh_animation.times[p1]) {
                break;
            }
        }
        if (p1 == 0) {
            p_result.values.push_back(attach_mesh_animation.values.front());
            continue;
        }
        if (p1 >= attach_mesh_animation.times.size()) {
            p_result.values.push_back(attach_mesh_animation.values.back());
            continue;
        }
        float p0_value = attach_mesh_animation.values[p1 - 1];
        float p1_value = attach_mesh_animation.values[p1];
        double p0_time = attach_mesh_animation.times[p1 - 1];
        double p1_time = attach_mesh_animation.times[p1];
        float scale = static_cast<float>((p_time - p0_time) / (p1_time - p0_time));
        p_result.values.push_back(p0_value + scale * (p1_value - p0_value));
    }
}

