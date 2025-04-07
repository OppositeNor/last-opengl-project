#include "texture/texture_2d_rgb_gpu.h"
#include "graphics.h"
#include "utils.hpp"
#include <stb_image.h>
#include <cstring>
#include <memory>

Texture2DRGBGPU::~Texture2DRGBGPU() {
    if (texture_id != 0) {
        GL_CALL(glDeleteTextures, 1, &texture_id);
    }
}

void Texture2DRGBGPU::create_empty_texture(int p_width, int p_height) {
    GL_CALL(glBindTexture, GL_TEXTURE_2D, get_texture_id());
    std::unique_ptr<unsigned char[]> data(new unsigned char[p_width * p_height * 3]);
    memset(data.get(), 0, p_width * p_height * 3 * sizeof(unsigned int));
    set_gpu_data(data.get());
}

void Texture2DRGBGPU::parse_img(const char* p_file_path) {
    std::unique_ptr<unsigned char[], decltype(&stbi_image_free)> img_data(
        stbi_load(to_unix_style_path(p_file_path).c_str(), &width, &height, nullptr, 3), stbi_image_free);
    if (img_data == nullptr) {
        throw std::runtime_error("Failed to load image at path: " + std::string(p_file_path));
    }
    set_gpu_data(img_data.get());
}

void Texture2DRGBGPU::bind() const {
    GL_CALL(glBindTexture, GL_TEXTURE_2D, texture_id);
}

void Texture2DRGBGPU::unbind() const {
    GL_CALL(glBindTexture, GL_TEXTURE_2D, 0);
}

unsigned int Texture2DRGBGPU::get_texture_id() {
    if (texture_id == 0) {
        GL_CALL(glGenTextures, 1, &texture_id);
    }
    return texture_id;
}

void Texture2DRGBGPU::set_gpu_data(unsigned char* p_data) {
    if (texture_id == 0) {
        GL_CALL(glGenTextures, 1, &texture_id);
    }
    GL_CALL(glBindTexture, GL_TEXTURE_2D, texture_id);
        GL_CALL(glBindTexture, GL_TEXTURE_2D, this->texture_id);
        GL_CALL(glTexImage2D, GL_TEXTURE_2D, 0, config.srgb ? GL_SRGB : GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, p_data);
        if (config.generate_mipmap) {
            GL_CALL(glGenerateMipmap, GL_TEXTURE_2D);
        }
        GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, config.scale_nearest_min ? GL_NEAREST : GL_LINEAR);
        GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, config.scale_nearest_mag ? GL_NEAREST : GL_LINEAR);
        set_wrap(config.wrap_mode_s, GL_TEXTURE_WRAP_S);
        set_wrap(config.wrap_mode_t, GL_TEXTURE_WRAP_T);
        GL_CALL(glBindTexture, GL_TEXTURE_2D, 0);
}
