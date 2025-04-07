#include "framebuffer/fb_regular.h"
#include "graphics.h"

FBRegular::FBRegular(std::size_t p_width, std::size_t p_height, const glm::vec4& p_clear_color, const glm::vec4& p_draw_clear_color,
                     const TextureConfigurations& p_config, std::shared_ptr<Shader> p_shader, std::shared_ptr<Shader> p_draw_shader)
        : shader(p_shader), draw_shader(p_draw_shader), width(p_width), height(p_height), clear_color(p_clear_color),
        draw_clear_color(p_draw_clear_color), vao(0), vbo(0) {
    GL_CALL(glGenFramebuffers, 1, &id);
    GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, id);
    GL_CALL(glGenTextures, 1, &texture_id);
    GL_CALL(glBindTexture, GL_TEXTURE_2D, texture_id);
    GL_CALL(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGBA8, p_width, p_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, p_config.scale_nearest_mag ? GL_NEAREST : GL_LINEAR);
    GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, p_config.scale_nearest_min ? GL_NEAREST : GL_LINEAR);
    set_wrap(p_config.wrap_mode_s, GL_TEXTURE_WRAP_S);
    set_wrap(p_config.wrap_mode_t, GL_TEXTURE_WRAP_T);
    GL_CALL(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);
    GL_CALL(glGenRenderbuffers, 1, &rbo_id);
    GL_CALL(glBindRenderbuffer, GL_RENDERBUFFER, rbo_id);
    GL_CALL(glRenderbufferStorage, GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, p_width, p_height);
    GL_CALL(glFramebufferRenderbuffer, GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_id);
    auto result = GL_CALL(glCheckFramebufferStatus, GL_FRAMEBUFFER);
    if (result != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Framebuffer is not complete");
    }
    GL_CALL(glEnable, GL_BLEND);
    GL_CALL(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GL_CALL(glBindTexture, GL_TEXTURE_2D, 0);
    GL_CALL(glBindRenderbuffer, GL_RENDERBUFFER, 0);
    GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
}

FBRegular::~FBRegular() {
    if (vao != 0) {
        GL_CALL(glDeleteVertexArrays, 1, &vao);
        GL_CALL(glDeleteBuffers, 1, &vbo);
    }
    GL_CALL(glDeleteFramebuffers, 1, &id);
    GL_CALL(glDeleteTextures, 1, &texture_id);
    GL_CALL(glDeleteRenderbuffers, 1, &rbo_id);
}

void FBRegular::bind() const {
    GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, id);
    GL_CALL(glClearColor, clear_color.r, clear_color.g, clear_color.b, clear_color.a);
    GL_CALL(glViewport, 0, 0, width, height);
    GL_CALL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FBRegular::bind_texture() const {
    GL_CALL(glBindTexture, GL_TEXTURE_2D, texture_id);
}

void FBRegular::unbind() const {
    GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
}

void FBRegular::update_size(unsigned int p_width, unsigned int p_height) {
    GL_CALL(glBindTexture, GL_TEXTURE_2D, texture_id);
    GL_CALL(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGBA8, p_width, p_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    GL_CALL(glBindRenderbuffer, GL_RENDERBUFFER, rbo_id);
    GL_CALL(glRenderbufferStorage, GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, p_width, p_height);
    GL_CALL(glBindTexture, GL_TEXTURE_2D, 0);
    GL_CALL(glBindRenderbuffer, GL_RENDERBUFFER, 0);
    width = p_width;
    height = p_height;
}

void FBRegular::draw(RenderData* p_render_data) const {
    if (draw_shader == nullptr) {
        return;
    }
    if (vao == 0) {
        init_draw_data();
    }
    GL_CALL(glViewport, 0, 0, width, height);
    GL_CALL(glClearColor, draw_clear_color.r, draw_clear_color.g, draw_clear_color.b, draw_clear_color.a);
    GL_CALL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GL_CALL(glEnable, GL_BLEND);
    GL_CALL(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    draw_shader->use();
    draw_shader->set_uniform("framebuffer", 0);
    GL_CALL(glActiveTexture, GL_TEXTURE0);
    GL_CALL(glBindTexture, GL_TEXTURE_2D, texture_id);
    draw_shader->set_uniform("depth_map", 1);
    GL_CALL(glActiveTexture, GL_TEXTURE1);
    p_render_data->depth_buffer->bind_texture();
    if (p_render_data->normal_buffer.has_value()) {
        draw_shader->set_uniform("normal_map", 2);
        GL_CALL(glActiveTexture, GL_TEXTURE2);
        p_render_data->normal_buffer.value()->bind_texture();
    }
    GL_CALL(glBindVertexArray, vao);
    GL_CALL(glDrawArrays, GL_TRIANGLES, 0, 6);
    GL_CALL(glBindVertexArray, 0);
    draw_shader->unuse();
}

void FBRegular::init_draw_data() const {
    GL_CALL(glGenVertexArrays, 1, &vao);
    GL_CALL(glBindVertexArray, vao);
    GL_CALL(glGenBuffers, 1, &vbo);
    GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vbo);
    GL_CALL(glBufferData, GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
    GL_CALL(glVertexAttribPointer, 0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    GL_CALL(glEnableVertexAttribArray, 0);
    GL_CALL(glVertexAttribPointer, 1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    GL_CALL(glEnableVertexAttribArray, 1);
    GL_CALL(glBindVertexArray, 0);
}
