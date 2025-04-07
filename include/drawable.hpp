#ifndef __DRAWABLE_HPP__
#define __DRAWABLE_HPP__

#include <memory>
#include <glm/glm.hpp>
class Drawable {
public:
    Drawable() = default;
    virtual ~Drawable() = default;

    virtual void draw(const struct RenderData& p_render_data) const = 0;

    virtual void draw_transform_only(std::shared_ptr<const class Shader> p_shader) const = 0;

    virtual void draw_flux(std::shared_ptr<const class SpotLight> p_spot_light) const = 0;

    virtual void draw_depth(const RenderData& p_render_data) const = 0;

    void disable_shadow() {
        should_cast_shadow = false;
    }

    void enable_shadow() {
        should_cast_shadow = true;
    }

    bool is_shadow_enabled() const {
        return should_cast_shadow;
    }

    void disable_lighting() {
        lighting_enabled = false;
    }

    void enable_lighting() {
        lighting_enabled = true;
    }

    bool is_lighting_enabled() const {
        return lighting_enabled;
    }
protected:
    bool should_cast_shadow = true;
    bool lighting_enabled = true;
};

#endif
