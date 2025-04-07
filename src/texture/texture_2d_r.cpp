#include "texture/texture_2d_r.h"
#include "utils.hpp"
#include "graphics.h"
#include <cstring>
#include <stb_image.h>

void Texture2DR::create_empty_texture(int p_width, int p_height) {
    if (texture_id != 0) {
        GL_CALL(glDeleteTextures, 1, &texture_id);
        texture_id = 0;
    }
    DataType temp_data(new unsigned char[p_width * p_height], free_data);
    memset(temp_data.get(), 0, p_width * p_height * sizeof(unsigned int));
    width = p_width;
    height = p_height;
    data = std::move(temp_data);
}

void Texture2DR::parse_img(const char* p_file_path) {
    DataType temp_data(stbi_load(to_unix_style_path(p_file_path).c_str(), &width, &height, nullptr, 1), (DataFreeFunc)stbi_image_free);
    if (temp_data == nullptr) {
        throw std::runtime_error("Failed to load image at path: " + std::string(p_file_path));
    }
    data = std::move(temp_data);
}

void Texture2DR::bind() const {
    GL_CALL(glBindTexture, GL_TEXTURE_2D, get_texture_id());
}

void Texture2DR::unbind() const {
    GL_CALL(glBindTexture, GL_TEXTURE_2D, 0);
}

void Texture2DR::set_texture_row(int p_row, const unsigned char* p_row_data) {
    for (int i = 0; i < width; ++i) {
        data[p_row * width + i] = p_row_data[i];
    }
    if (texture_id != 0) {
        GL_CALL(glBindTexture, GL_TEXTURE_2D, get_texture_id());
        GL_CALL(glTexSubImage2D, GL_TEXTURE_2D, 0, 0, p_row, width, 1, GL_RED, GL_UNSIGNED_BYTE, p_row_data);
        GL_CALL(glBindTexture, GL_TEXTURE_2D, 0);
    }
}

unsigned int Texture2DR::get_texture_id() const {
    if (texture_id == 0) {
        GL_CALL(glGenTextures, 1, &this->texture_id);
        GL_CALL(glBindTexture, GL_TEXTURE_2D, this->texture_id);
        GL_CALL(glTexImage2D, GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data.get());
        if (config.generate_mipmap) {
            GL_CALL(glGenerateMipmap, GL_TEXTURE_2D);
        }
        GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, config.scale_nearest_min ? GL_NEAREST : GL_LINEAR);
        GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, config.scale_nearest_mag ? GL_NEAREST : GL_LINEAR);
        set_wrap(config.wrap_mode_s, GL_TEXTURE_WRAP_S);
        set_wrap(config.wrap_mode_t, GL_TEXTURE_WRAP_T);
        GL_CALL(glBindTexture, GL_TEXTURE_2D, 0);
    }
    return texture_id;
}
