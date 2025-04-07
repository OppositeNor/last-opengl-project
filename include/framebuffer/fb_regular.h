#ifndef __FB_REGULAR_H__
#define __FB_REGULAR_H__

#include "framebuffer/framebuffer.h"
#include "shader/shader.h"

class FBRegular : public Framebuffer {
public:

    FBRegular(std::size_t p_width, std::size_t p_height, const glm::vec4& p_clear_color, const glm::vec4& p_draw_clear_color,
              const TextureConfigurations& p_config, std::shared_ptr<Shader> p_shader, std::shared_ptr<Shader> p_draw_shader = nullptr);

    virtual ~FBRegular() override;

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

    // Get the shader for the objects rendering
    virtual std::shared_ptr<const Shader> get_shader() const override {
        return shader;
    }

    // Get the shader for the objects rendering
    virtual std::shared_ptr<Shader> get_shader() override {
        return shader;
    }

    // Get the draw shader for the framebuffer rendering
    virtual std::shared_ptr<const Shader> get_draw_shader() const override {
        return draw_shader;
    }

    // Get the draw shader for the framebuffer rendering
    virtual std::shared_ptr<Shader> get_draw_shader() override {
        return draw_shader;
    }

    /**
     * @brief Draw the framebuffer.
     */
    virtual void draw(RenderData* render_data) const override;

private:
    std::shared_ptr<Shader> shader;
    std::shared_ptr<Shader> draw_shader;
    std::size_t width, height;
    glm::vec4 clear_color;
    glm::vec4 draw_clear_color;

    unsigned int id;
    unsigned int texture_id;
    unsigned int rbo_id;
    mutable unsigned int vao, vbo;

    void init_draw_data() const;
};

#endif
