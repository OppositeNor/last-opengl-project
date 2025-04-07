#ifndef _LIGHT_H_
#define _LIGHT_H_
#include "framebuffer/cube_depth_framebuffer.h"
#include "shader/shader.h"
#include <glm/glm.hpp>

class PointLight {
public:
    constexpr static int MAX_COUNT = 4;
    constexpr static float SHADOW_MAP_BIAS = 0.08;
    constexpr static float SHADOW_FILTER_SIZE = 0.01;
    constexpr static int SHADOW_FILTER_COUNT = 4;

    PointLight()
        : PointLight(glm::vec3(1.0f), 10.0f) {}

    PointLight(const glm::vec3& p_color, float p_attenuation, unsigned int p_shadow_map_resolution = 2048, float p_shadow_range = 80.0f);

    ~PointLight();

    // Get the position of the light
    const glm::vec3& get_position() const {
        return position;
    }

    // Get the position of the light
    glm::vec3& get_position() {
        return position;
    }

    // Get the color of the light
    const glm::vec3& get_color() const {
        return color;
    }

    // Get the color of the light
    glm::vec3& get_color() {
        return color;
    }

    float get_attenuation() const {
        return attenuation;
    }

    float& get_attenuation() {
        return attenuation;
    }

    void bind_shadow_map() const {
        shadow_map->bind_cube();
    }

    void unbind_shadow_map() const {
        shadow_map->unbind();
    }

    // Get the view projection matrix
    glm::mat4 get_vp(int index) const;

    // Get the shadow range
    float get_shadow_range() const {
        return shadow_range;
    }

    void pre_draw() const {
        shadow_map->bind();
        for (std::size_t i = 0; i < 6; i++) {
            depth_shader->set_uniform("vps[" + std::to_string(i) + "]", get_vp(i));
        }
        depth_shader->set_uniform("far_plane", shadow_range);
        depth_shader->set_uniform("light_pos", position);
    }

    void post_draw() const {
        depth_shader->unuse();
        unbind_shadow_map();
    }

    std::shared_ptr<const Shader> get_depth_shader() const {
        return depth_shader;
    }


private:
    float shadow_range;
    CubeDepthFramebuffer* shadow_map;
    std::shared_ptr<Shader> depth_shader;
    glm::vec3 position;
    glm::vec3 color;
    float attenuation;
    glm::mat4 proj_matrices[6];

    enum {
        SHADOW_MAP_RIGHT = 0,
        SHADOW_MAP_LEFT,
        SHADOW_MAP_UP,
        SHADOW_MAP_DOWN,
        SHADOW_MAP_FRONT,
        SHADOW_MAP_BACK
    };
};

class PointLightObject {
public:
    virtual std::shared_ptr<PointLight> get_light() = 0;
    virtual std::shared_ptr<const PointLight> get_light() const = 0;
    virtual void update_point_light() const = 0;
};

#endif
