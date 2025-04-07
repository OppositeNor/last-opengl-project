#include "texture/texture_3d.h"
#include "graphics.h"

void Texture3D::set_wrap(TextureConfigurations::WrapMode p_wrap_mode, unsigned int p_wrap_dimension) {
    switch (p_wrap_mode) {
        case TextureConfigurations::WrapMode::REPEAT:
            GL_CALL(glTexParameteri, GL_TEXTURE_3D, p_wrap_dimension, GL_REPEAT);
            break;
        case TextureConfigurations::WrapMode::MORRORED_REPEAT:
            GL_CALL(glTexParameteri, GL_TEXTURE_3D, p_wrap_dimension, GL_MIRRORED_REPEAT);
            break;
        case TextureConfigurations::WrapMode::CLAMP_TO_EDGE:
            GL_CALL(glTexParameteri, GL_TEXTURE_3D, p_wrap_dimension, GL_CLAMP_TO_EDGE);
            break;
        case TextureConfigurations::WrapMode::CLAMP_TO_BORDER:
            GL_CALL(glTexParameteri, GL_TEXTURE_3D, p_wrap_dimension, GL_CLAMP_TO_BORDER);
            break;
    };
}
