#include "scene_node/scene_node.h"

void SceneNode::add_child(SceneNode* p_child) {
    p_child->parent = this;
    children.insert(p_child);
}

const Transform& SceneNode::get_local_transform() const {
    return local_transform;
}

Transform& SceneNode::get_local_transform() {
    return local_transform;
}

const glm::mat4& SceneNode::get_global_transform() const {
    update_global_transform();
    return global_transform;
}

void SceneNode::update_global_transform() const {
    if (parent != nullptr) {
        global_transform = parent->global_transform * local_transform.get_matrix();
    } else {
        global_transform = local_transform.get_matrix();
    }
}

void SceneNode::update_all_global_transform(bool p_tick_tock) const {
    if (p_tick_tock & global_transform_tick_tock) {
        return;
    }
    if (parent != nullptr) {
        parent->update_all_global_transform(p_tick_tock);
        global_transform = parent->global_transform * local_transform.get_matrix();
    } else {
        global_transform = local_transform.get_matrix();
    }
    global_transform_tick_tock = p_tick_tock;
}

