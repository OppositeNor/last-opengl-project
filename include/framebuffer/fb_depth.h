#ifndef __FB_DEPTH_H__
#define __FB_DEPTH_H__
#include "framebuffer/framebuffer.h"
#include "utils.hpp"
class FBDepth : public Framebuffer {
public:
    FBDepth(std::size_t p_width, std::size_t p_height, const TextureConfigurations& p_config, std::shared_ptr<class Shader> p_depth_shader,
            std::shared_ptr<class Shader> p_draw_shader = nullptr);

    virtual ~FBDepth() override;

    /**
     * @brief Bind the framebuffer.
     */
    virtual void bind() const override;

    /**
     * @brief Bind the texture.
     */
    virtual void bind_texture() const override;

    /**
     * @brief Unbind the framebuffer.
     */
    virtual void unbind() const override;

    /**
     * @brief Update the size of the framebuffer.
     *
     * @param p_width Width
     * @param p_height Height
     */
    virtual void update_size(unsigned int p_width, unsigned int p_height) override;

    /**
     * @brief Draw the framebuffer.
     */
    virtual void draw(RenderData* render_data) const override;

    // Get the shader for the objects rendering
    virtual std::shared_ptr<const Shader> get_shader() const override { return depth_shader; }

    // Get the shader for the objects rendering
    virtual std::shared_ptr<Shader> get_shader() override { return depth_shader; }

    // Get the draw shader for the framebuffer rendering
    virtual std::shared_ptr<const Shader> get_draw_shader() const override { return draw_shader; }

    // Get the draw shader for the framebuffer rendering
    virtual std::shared_ptr<Shader> get_draw_shader() override { return draw_shader; }

private:
    unsigned int id;
    std::size_t width, height;
    std::shared_ptr<Shader> depth_shader;
    std::shared_ptr<Shader> draw_shader;
    unsigned int texture_id;
    mutable unsigned int vao, vbo;
    void init_draw_data() const;
};

#endif
