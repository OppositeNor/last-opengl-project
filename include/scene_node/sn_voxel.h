#ifndef __SN_VOXEL_H__
#define __SN_VOXEL_H__

#include "scene_node/scene_node.h"
#include "utils.hpp"
#include "voxel/voxel.hpp"
class SNVoxel : public SceneNode {
public:
    SNVoxel(Application* p_application, std::shared_ptr<Voxel> p_voxel)
        : SceneNode(p_application), voxel(REQUIRE_NON_NULL(p_voxel)) {}
    virtual ~SNVoxel() override {}

private:
    std::shared_ptr<Voxel> voxel;
};

#endif
