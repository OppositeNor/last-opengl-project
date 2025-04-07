#ifndef __SPOT_LIGHT_H__
#define __SPOT_LIGHT_H__

#include "framebuffer/fb_depth.h"
#include "framebuffer/fb_srgb.h"
#include "framebuffer/fb_regular.h"
#include "scene_grid/sg_lpv_sh.h"
#include "utils.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

class SpotLight {
public:
    constexpr static int MAX_COUNT = 4;
    constexpr static int SHADOW_FILTER_SIZE = 12;
    constexpr static int RSM_SAMPLE_COUNT = 16;
    constexpr static float RSM_SAMPLE_RMAX = 0.5;

    enum GIType {
        NONE,
        RSM,
        LPV
    };

    SpotLight(const glm::vec3& p_color, float p_attenuation, float p_shadow_range, unsigned int p_shadow_resolution, const glm::mat4& p_proj_mat,
              float p_shadow_map_bias, unsigned int p_rsm_resolution, unsigned int p_lpv_resolution, const glm::vec3& p_lpv_range, GIType p_gi_type);
    virtual ~SpotLight();

    class SpotLightBuilder {
    public:
        SpotLightBuilder() {}
        ~SpotLightBuilder() {}

        SpotLightBuilder& set_prop(const glm::vec3& p_color, float p_attenuation, float p_shadow_range, unsigned int p_shadow_map_resolution) {
            color = p_color;
            attenuation = p_attenuation;
            shadow_range = p_shadow_range;
            shadow_map_resolution = p_shadow_map_resolution;
            return *this;
        }

        SpotLightBuilder& set_spot_light(float p_angle, float p_near) {
            proj_mat = glm::perspective(p_angle, 1.0f, p_near, UNPACK_OPTIONAL(shadow_range));
            return *this;
        }

        SpotLightBuilder& set_rsm_gi(unsigned int p_rsm_resolution) {
            gi_type = GIType::RSM;
            rsm_resolution = p_rsm_resolution;
            return *this;
        }

        SpotLightBuilder& set_lpv_gi(unsigned int p_rsm_resolution, unsigned int p_lpv_resolution, const glm::vec3& p_lpv_range) {
            gi_type = GIType::LPV;
            rsm_resolution = p_rsm_resolution;
            lpv_resolution = p_lpv_resolution;
            lpv_range = p_lpv_range;
            return *this;
        }

        SpotLightBuilder& set_shadow_map_bias(float p_shadow_map_bias) {
            shadow_map_bias = p_shadow_map_bias;
            return *this;
        }

        std::shared_ptr<SpotLight> build_shared() const {
            return std::make_shared<SpotLight>(UNPACK_OPTIONAL(color), UNPACK_OPTIONAL(attenuation), UNPACK_OPTIONAL(shadow_range),
                                               UNPACK_OPTIONAL(shadow_map_resolution), UNPACK_OPTIONAL(proj_mat),
                                               shadow_map_bias, rsm_resolution, lpv_resolution, lpv_range, gi_type);
        }

    private:
        std::optional<glm::vec3> color;
        std::optional<float> attenuation;
        std::optional<float> shadow_range;
        std::optional<unsigned int> shadow_map_resolution;
        std::optional<glm::mat4> proj_mat;
        GIType gi_type = GIType::NONE;
        unsigned int rsm_resolution = 0;
        unsigned int lpv_resolution = 0;
        glm::vec3 lpv_range;
        float shadow_map_bias = 0.0004;
    };

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

    const glm::vec3& get_position() const {
        return position;
    }

    const glm::mat4& get_shadow_map_proj() const {
        return proj_mat;
    }

    glm::mat4& get_shadow_map_porj() {
        return proj_mat;
    }

    const glm::mat4& get_light_view() const {
        return view_mat;
    }

    void bind_shadow_map() const {
        shadow_map->bind_texture();
    }

    void pre_draw(Framebuffer* p_fb) const {
        REQUIRE_NON_NULL(p_fb)->bind();
        p_fb->get_shader()->use();
        p_fb->get_shader()->set_uniform("proj", proj_mat);
        p_fb->get_shader()->set_uniform("view", view_mat);
        p_fb->get_shader()->set_uniform("far_plane", shadow_range);
        p_fb->enable_depth_test();
    }

    void post_draw() const {
        shadow_map->get_shader()->unuse();
        shadow_map->unbind();
    }

    // Get the shadow range
    float get_shadow_range() const {
        return shadow_range;
    }

    void update_light_space(const glm::mat4& p_light_local) {
        view_mat = glm::inverse(p_light_local);
        position = p_light_local * glm::vec4(0, 0, 0, 1);
    }

    glm::mat4 get_light_space() const {
        return proj_mat * view_mat;
    }

    const std::unique_ptr<FBDepth>& get_shadow_map() const {
        return shadow_map;
    }

    GIType get_gi_type() const {
        return gi_type;
    }

    float get_shadow_map_bias() const {
        return shadow_map_bias;
    }

    void set_shadow_map_bias(float p_shadow_map_bias) {
        shadow_map_bias = p_shadow_map_bias;
    }

    const std::unique_ptr<FBFRGB>& get_coordinate_map() const {
        return coordinate_map;
    }

    const std::unique_ptr<FBRegular>& get_normal_map() const {
        return normal_map;
    }

    const std::unique_ptr<FBFRGB>& get_flux_map() const {
        return flux_map;
    }

    unsigned int get_shadow_map_resolution() const {
        return shadow_map_resolution;
    }

    unsigned int get_rsm_resolution() const {
        return rsm_resolution;
    }

    unsigned int get_lpv_resolution() const {
        return lpv_resolution;
    }

    const SGLPVSH* get_sg_lpv() const {
        return sg_lpv_sh.get();
    }

private:
    glm::vec3 position;
    glm::vec3 color;
    float attenuation;
    float shadow_range;
    unsigned int shadow_map_resolution;
    glm::mat4 view_mat;
    glm::mat4 proj_mat;

    std::unique_ptr<FBDepth> shadow_map = nullptr;
    std::unique_ptr<FBFRGB> coordinate_map = nullptr;
    std::unique_ptr<FBRegular> normal_map = nullptr;
    std::unique_ptr<FBFRGB> flux_map = nullptr;
    float shadow_map_bias;
    GIType gi_type;
    unsigned int rsm_resolution;
    unsigned int lpv_resolution;
    glm::vec3 lpv_range;
    std::unique_ptr<SGLPVSH> sg_lpv_sh = nullptr;
};

class SpotLightObject {
public:
    // Get the light
    virtual std::shared_ptr<SpotLight> get_light() = 0;

    // Get the light
    virtual std::shared_ptr<const SpotLight> get_light() const = 0;

    // updates the area light
    virtual void update_spot_light() const = 0;
};

#endif
