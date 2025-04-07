#include "framebuffer/framebuffer.h"
#include "graphics.h"

const float Framebuffer::vertex_data[24] {
    -1.0f, -1.0f, 0.0f, 0.0f,
     1.0f, -1.0f, 1.0f, 0.0f,
     1.0f,  1.0f, 1.0f, 1.0f,
    
    -1.0f,  1.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f,
     1.0f,  1.0f, 1.0f, 1.0f
};

void Framebuffer::enable_depth_test() {
    GL_CALL(glEnable, GL_DEPTH_TEST);
}

void Framebuffer::disable_depth_test() {
    GL_CALL(glDisable, GL_DEPTH_TEST);
}

void Framebuffer::set_wrap(TextureConfigurations::WrapMode p_wrap_mode, unsigned int p_wrap_dimension) {
    switch (p_wrap_mode) {
        case TextureConfigurations::WrapMode::REPEAT:
            GL_CALL(glTexParameteri, GL_TEXTURE_2D, p_wrap_dimension, GL_REPEAT);
            break;
        case TextureConfigurations::WrapMode::MORRORED_REPEAT:
            GL_CALL(glTexParameteri, GL_TEXTURE_2D, p_wrap_dimension, GL_MIRRORED_REPEAT);
            break;
        case TextureConfigurations::WrapMode::CLAMP_TO_EDGE:
            GL_CALL(glTexParameteri, GL_TEXTURE_2D, p_wrap_dimension, GL_CLAMP_TO_EDGE);
            break;
        case TextureConfigurations::WrapMode::CLAMP_TO_BORDER:
            GL_CALL(glTexParameteri, GL_TEXTURE_2D, p_wrap_dimension, GL_CLAMP_TO_BORDER);
            break;
    };
}
