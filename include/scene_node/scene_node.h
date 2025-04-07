#ifndef __SCENE_NODE_H__
#define __SCENE_NODE_H__
#include "application/application.h"
#include "transform.h"
#include "utils.hpp"

class SceneNode {
public:
    SceneNode(Application* p_application) : application(REQUIRE_NON_NULL(p_application)) {}
    SceneNode(const SceneNode&) = delete;
    virtual ~SceneNode() = default;

    // Add a child node
    void add_child(SceneNode* p_child);

    // Get the local transform
    const Transform& get_local_transform() const;

    // Get the local transform
    Transform& get_local_transform();

    // Get the global transform
    const glm::mat4& get_global_transform() const;
    void update_global_transform() const;

    void update_all_global_transform(bool p_tick_tock) const;
protected:
    mutable glm::mat4 global_transform;
    Transform local_transform;
    SceneNode* parent = nullptr;
    std::unordered_set<SceneNode*> children;
    Application* application;
    mutable bool global_transform_tick_tock = false;
};

template <typename T>
class SceneNodeBase : public SceneNode {
public:
    SceneNodeBase(Application* p_application)
            : SceneNode(p_application) {
        application->add_scene_node(static_cast<T*>(this));
    }

    virtual ~SceneNodeBase() override {
        application->remove_scene_node(static_cast<T*>(this));
    }
};

#endif
