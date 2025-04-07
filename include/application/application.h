#ifndef __APPLICATION_H__
#define __APPLICATION_H__
#include <unordered_set>
#include "utils.hpp"

class Application {
public:
    Application() = default;
    virtual ~Application() {}

    Application(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(const Application&) = delete;
    Application& operator=(Application&&) = delete;

    // Initializes the application.
    virtual void init(const char* p_window_title, int p_window_width, int p_window_height) = 0;

    // Returns true if the window should be closed.
    virtual bool window_should_close() = 0;

    // Called every fram in the main loop.
    virtual void process(double p_delta_time) = 0;

    // Terminates the application.
    virtual void terminate() = 0;

    void tick(double p_delta_time);

    template<typename T>
    void add_scene_node(const T* p_scene_node) {
        scene_nodes.insert(p_scene_node);
        using type = SceneNodeType<T>;
        if constexpr (type::is_drawable) {
            drawables.insert(p_scene_node);
        }
        if constexpr (type::is_animated_object) {
            animated_objects.insert(p_scene_node);
        }
        if constexpr (type::is_point_light) {
            point_light_objects.insert(p_scene_node);
        }
        if constexpr (type::is_spot_light) {
            spot_light_objects.insert(p_scene_node);
        }
    }

    template<typename T>
    void remove_scene_node(const T* p_scene_node) {
        scene_nodes.erase(p_scene_node);
        using type = SceneNodeType<T>;
        if constexpr (type::is_drawable) {
            drawables.erase(p_scene_node);
        }
        if constexpr (type::is_animated_object) {
            animated_objects.erase(p_scene_node);
        }
        if constexpr (type::is_point_light) {
            point_light_objects.erase(p_scene_node);
        }
        if constexpr (type::is_spot_light) {
            spot_light_objects.erase(p_scene_node);
        }
    }

protected:
    std::unordered_set<const Drawable*> drawables;
    std::unordered_set<const AnimatedObject*> animated_objects;
    std::unordered_set<const PointLightObject*> point_light_objects;
    std::unordered_set<const SpotLightObject*> spot_light_objects;
    std::unordered_set<const class SceneNode*> scene_nodes;
    bool tick_tock = false;
};

#endif
