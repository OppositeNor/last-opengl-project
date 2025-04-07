#ifndef __FB_FRGB_H__
#define __FB_FRGB_H__
#include "framebuffer/framebuffer.h"
class FBFRGB : public Framebuffer {
public:
    FBFRGB(std::size_t p_width, std::size_t p_height, const glm::vec3& p_clear_color, const glm::vec3& p_draw_clear_color,
           const TextureConfigurations& p_config, std::shared_ptr<Shader> p_shader, std::shared_ptr<Shader> p_draw_shader = nullptr);

    virtual ~FBFRGB();

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
    virtual void draw(struct RenderData* p_render_data) const override;

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

private:
    std::shared_ptr<Shader> shader;
    std::shared_ptr<Shader> draw_shader;
    std::size_t width, height;
    glm::vec3 clear_color;
    glm::vec3 draw_clear_color;

    unsigned int id;
    unsigned int texture_id;
    unsigned int rbo_id;
    mutable unsigned int vao, vbo;

    TextureConfigurations config;
    void init_draw_data() const;
};
#endif
