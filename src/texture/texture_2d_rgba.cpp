#include "texture/texture_2d_rgba.h"
#include "graphics.h"
#include "utils.hpp"
#include <cstring>
#include <stb_image.h>
#include <stdexcept>
#include <sstream>

Texture2DRGBA::~Texture2DRGBA() {
    if (texture_id != 0) {
        GL_CALL(glDeleteTextures, 1, &texture_id);
    }
}

void Texture2DRGBA::load() {
    parse_img(file_path.c_str());
}

void Texture2DRGBA::create_empty_texture(int p_width, int p_height) {
    if (texture_id != 0) {
        GL_CALL(glDeleteTextures, 1, &texture_id);
        texture_id = 0;
    }
    DataType temp_data(new unsigned char[p_width * p_height * 4], free_data);
    memset(temp_data.get(), 0, p_width * p_height * 4 * sizeof(unsigned char));
    data = std::move(temp_data);
    width = p_width;
    height = p_height;
}

void Texture2DRGBA::parse_img(const char* p_file_path) {
    DataType temp_data(stbi_load(to_unix_style_path(p_file_path).c_str(), &width, &height, nullptr, 4), (DataFreeFunc)stbi_image_free);
    if (temp_data == nullptr) {
        throw std::runtime_error("Failed to load image at path: " + std::string(p_file_path));
    }
    data = std::move(temp_data);
}

void Texture2DRGBA::bind() const {
    GL_CALL(glBindTexture, GL_TEXTURE_2D, get_texture_id());
}

void Texture2DRGBA::unbind() const {
    GL_CALL(glBindTexture, GL_TEXTURE_2D, 0);
}

unsigned int Texture2DRGBA::get_texture_id() const {
    if (texture_id == 0) {
        GL_CALL(glGenTextures, 1, &this->texture_id);
        GL_CALL(glBindTexture, GL_TEXTURE_2D, this->texture_id);
        GL_CALL(glTexImage2D, GL_TEXTURE_2D, 0, config.srgb ? GL_SRGB_ALPHA : GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.get());
        if (config.generate_mipmap) {
            GL_CALL(glGenerateMipmap, GL_TEXTURE_2D);
        }
        GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, config.scale_nearest_min ? GL_NEAREST : GL_LINEAR);
        GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, config.scale_nearest_mag ? GL_NEAREST : GL_LINEAR);
        set_wrap(config.wrap_mode_s, GL_TEXTURE_WRAP_S);
        set_wrap(config.wrap_mode_t, GL_TEXTURE_WRAP_T);
        GL_CALL(glBindTexture, GL_TEXTURE_2D, 0);
    }
    return this->texture_id;
}

void Texture2DRGBA::set_texture_row(int p_row, const unsigned char* p_row_data) {
    if (p_row < 0 || p_row >= height) {
        std::stringstream ss;
        ss << "Row out of bounds: " << p_row;
        throw std::runtime_error(ss.str());
    }
    for (int i = 0; i < width * 4; ++i) {
        data[p_row * width * 4 + i] = p_row_data[i];
    }
    if (texture_id != 0) {
        GL_CALL(glBindTexture, GL_TEXTURE_2D, get_texture_id());
        GL_CALL(glTexSubImage2D, GL_TEXTURE_2D, 0, 0, p_row, width, 1, GL_RGBA, GL_UNSIGNED_BYTE, p_row_data);
        GL_CALL(glBindTexture, GL_TEXTURE_2D, 0);
    }
}
