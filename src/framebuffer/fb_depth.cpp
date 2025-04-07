#include "framebuffer/fb_depth.h"
#include "graphics.h"
#include "texture/texture.hpp"

FBDepth::FBDepth(std::size_t p_width, std::size_t p_height, const TextureConfigurations& p_config, std::shared_ptr<Shader> p_depth_shader,
                 std::shared_ptr<Shader> p_draw_shader)
    : width(p_width), height(p_height), draw_shader(p_draw_shader), vao(0), vbo(0) {
    GL_CALL(glGenFramebuffers, 1, &id);
    GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, id);
    GL_CALL(glGenTextures, 1, &texture_id);
    GL_CALL(glBindTexture, GL_TEXTURE_2D, texture_id);
    GL_CALL(glTexImage2D, GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, p_config.scale_nearest_mag ? GL_NEAREST : GL_LINEAR);
    GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, p_config.scale_nearest_min ? GL_NEAREST : GL_LINEAR);
    set_wrap(p_config.wrap_mode_s, GL_TEXTURE_WRAP_S);
    set_wrap(p_config.wrap_mode_t, GL_TEXTURE_WRAP_T);
    GL_CALL(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture_id, 0);

    GL_CALL(glDrawBuffer, GL_NONE);
    GL_CALL(glReadBuffer, GL_NONE);

    auto result = GL_CALL(glCheckFramebufferStatus, GL_FRAMEBUFFER);
    if (result != GL_FRAMEBUFFER_COMPLETE) {
        GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
        GL_CALL(glDeleteFramebuffers, 1, &id);
        GL_CALL(glDeleteTextures, 1, &texture_id);
        throw std::runtime_error("Framebuffer is not complete");
    }

    GL_CALL(glBindTexture, GL_TEXTURE_2D, 0);
    GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER,  0);
    depth_shader = p_depth_shader;
}

FBDepth::~FBDepth() {
    if (vao != 0) {
        GL_CALL(glDeleteVertexArrays, 1, &vao);
        GL_CALL(glDeleteBuffers, 1, &vbo);
    }
    GL_CALL(glDeleteTextures, 1, &texture_id);
    GL_CALL(glDeleteFramebuffers, 1, &id);
}

void FBDepth::bind() const {
    GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, id);
    GL_CALL(glViewport, 0, 0, width, height);
    GL_CALL(glClear, GL_DEPTH_BUFFER_BIT);
}

void FBDepth::bind_texture() const {
    GL_CALL(glBindTexture, GL_TEXTURE_2D, texture_id);
}

void FBDepth::unbind() const {
    GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
}

void FBDepth::update_size(unsigned int p_width, unsigned int p_height) {
    GL_CALL(glBindTexture, GL_TEXTURE_2D, texture_id);
    GL_CALL(glTexImage2D, GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
            p_width, p_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    width = p_width;
    height = p_height;
}

void FBDepth::draw(RenderData*) const {
    if (draw_shader == nullptr) {
        return;
    }
    if (vao == 0) {
        init_draw_data();
    }
    GL_CALL(glEnable, GL_DEPTH_TEST);
    GL_CALL(glViewport, 0, 0, width, height);
    GL_CALL(glClearColor, 0.0f, 0.0f, 0.0f, 0.0f);
    GL_CALL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GL_CALL(glEnable, GL_BLEND);
    GL_CALL(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    draw_shader->use();
    draw_shader->set_uniform("framebuffer", 0);
    GL_CALL(glActiveTexture, GL_TEXTURE0);
    GL_CALL(glBindTexture, GL_TEXTURE_2D, texture_id);
    GL_CALL(glBindVertexArray, vao);
    GL_CALL(glDrawArrays, GL_TRIANGLES, 0, 6);
    
    GL_CALL(glBindVertexArray, 0);
}

void FBDepth::init_draw_data() const {
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

