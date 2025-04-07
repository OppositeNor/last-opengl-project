#ifndef __MESH_MORPH_ANIMATION_H__
#define __MESH_MORPH_ANIMATION_H__
#include <vector>

struct MeshAnimationValue {
    std::vector <unsigned int> mesh_indices;
    std::vector<unsigned int> indices;
    std::vector<float> values;
};

struct AttachMeshAnimation {
    unsigned int index;
    std::vector<float> values;
    std::vector<double> times;
};

struct SingleMeshAnimation {
    std::vector<unsigned int> mesh_indices;
    std::vector<AttachMeshAnimation> attach_mesh_animations;
    void interpolate_values(double p_time, MeshAnimationValue& p_result) const;
};
#endif
