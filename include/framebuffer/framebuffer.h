#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__
#include "shader/shader.h"
#include "texture/texture.hpp"
#include <memory>

class Framebuffer {
public:
    Framebuffer() = default;

    virtual ~Framebuffer() {}

    /**
     * @brief Bind the framebuffer.
     */
    virtual void bind() const = 0;

    /**
     * @brief Bind the texture.
     */
    virtual void bind_texture() const = 0;

    /**
     * @brief Unbind the framebuffer.
     */
    virtual void unbind() const = 0;

    /**
     * @brief Update the size of the framebuffer.
     *
     * @param p_width Width
     * @param p_height Height
     */
    virtual void update_size(unsigned int p_width, unsigned int p_height) = 0;

    /**
     * @brief Draw the framebuffer.
     */
    virtual void draw(struct RenderData* p_render_data) const = 0;

    // Get the shader for the objects rendering
    virtual std::shared_ptr<const Shader> get_shader() const = 0;
    //
    // Get the shader for the objects rendering
    virtual std::shared_ptr<Shader> get_shader() = 0;

    // Get the draw shader for the framebuffer rendering
    virtual std::shared_ptr<const Shader> get_draw_shader() const = 0;

    // Get the draw shader for the framebuffer rendering
    virtual std::shared_ptr<Shader> get_draw_shader() = 0;

    // Enable depth test
    virtual void enable_depth_test();

    // Disable depth test
    virtual void disable_depth_test();

protected:
    static const float vertex_data[24];
    void set_wrap(TextureConfigurations::WrapMode p_wrap_mode, unsigned int p_wrap_dimension);
};
#endif
