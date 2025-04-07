#include "texture/texture_3d_frgba.h"
#include "graphics.h"
#include <cstring>
#include <memory>

Texture3DFRGBA::Texture3DFRGBA(unsigned int p_width, unsigned int p_height, unsigned int p_depth, const TextureConfigurations& p_config)
    : id(0), config(p_config) {
    width = p_width;
    height = p_height;
    depth = p_depth;
    GL_CALL(glGenTextures, 1, &id);
    GL_CALL(glBindTexture, GL_TEXTURE_3D, id);
    std::unique_ptr<float> temp_data(new float[width * height * depth * 4]);
    memset(temp_data.get(), 0, sizeof(float) * width * height * depth * 4);
    GL_CALL(glTexImage3D, GL_TEXTURE_3D, 0, GL_RGBA32F, width, height, depth, 0, GL_RGBA, GL_FLOAT, temp_data.get());
    temp_data.reset();
    if (config.generate_mipmap) {
        GL_CALL(glGenerateMipmap, GL_TEXTURE_3D);
    }
    GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, config.scale_nearest_min ? GL_NEAREST : GL_LINEAR);
    GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, config.scale_nearest_mag ? GL_NEAREST : GL_LINEAR);
    set_wrap(config.wrap_mode_s, GL_TEXTURE_WRAP_S);
    set_wrap(config.wrap_mode_t, GL_TEXTURE_WRAP_T);
    set_wrap(config.wrap_mode_r, GL_TEXTURE_WRAP_R);
    GL_CALL(glBindTexture, GL_TEXTURE_3D, 0);
}

Texture3DFRGBA::~Texture3DFRGBA() {
    GL_CALL(glDeleteTextures, 1, &id);
}

void Texture3DFRGBA::create_empty_texture(int p_width, int p_height, int p_depth) {
    width = p_width; height = p_height; depth = p_depth;
    GL_CALL(glBindTexture, GL_TEXTURE_3D, id);
    std::unique_ptr<float> temp_data(new float[width * height * depth * 4]);
    memset(temp_data.get(), 0, sizeof(float) * width * height * depth * 4);
    GL_CALL(glTexImage3D, GL_TEXTURE_3D, 0, GL_RGBA32F, width, height, depth, 0, GL_RGBA, GL_FLOAT, temp_data.get());
    temp_data.reset();
    if (config.generate_mipmap) {
        GL_CALL(glGenerateMipmap, GL_TEXTURE_3D);
    }
    GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, config.scale_nearest_min ? GL_NEAREST : GL_LINEAR);
    GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, config.scale_nearest_mag ? GL_NEAREST : GL_LINEAR);
    set_wrap(config.wrap_mode_s, GL_TEXTURE_WRAP_S);
    set_wrap(config.wrap_mode_t, GL_TEXTURE_WRAP_T);
    set_wrap(config.wrap_mode_r, GL_TEXTURE_WRAP_R);
    GL_CALL(glBindTexture, GL_TEXTURE_3D, 0);
}

void Texture3DFRGBA::bind() const {
    GL_CALL(glBindTexture, GL_TEXTURE_3D, id);
}

void Texture3DFRGBA::unbind() const {
    GL_CALL(glBindTexture, GL_TEXTURE_3D, 0);
}

void Texture3DFRGBA::bind_compute(unsigned int p_unit, int p_mipmap_level) const {
    GL_CALL(glBindImageTexture, p_unit, id, p_mipmap_level, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);
}

void Texture3DFRGBA::unbind_compute(unsigned int p_unit) const {
    GL_CALL(glBindImageTexture, p_unit, 0, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);
}
