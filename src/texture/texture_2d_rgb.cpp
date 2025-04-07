#include "texture/texture_2d_rgb.h"
#include "graphics.h"
#include "utils.hpp"
#include <sstream>
#include <cstring>
#include <stb_image.h>

Texture2DRGB::Texture2DRGB(const TextureConfigurations& p_config, const std::string& p_path)
    : path(p_path), config(p_config), data(nullptr, free_data) {
    width = 0;
    height = 0;
    texture_id = 0;
}

Texture2DRGB::~Texture2DRGB() {
    if (texture_id != 0) {
        GL_CALL(glDeleteTextures, 1, &texture_id);
    }
}

void Texture2DRGB::load() {
    parse_img(path.c_str());
}

void Texture2DRGB::create_empty_texture(int p_width, int p_height) {
    if (texture_id != 0) {
        GL_CALL(glDeleteTextures, 1, &texture_id);
        texture_id = 0;
    }
    DataType temp_data(new unsigned char[p_width * p_height * 3], free_data);
    memset(temp_data.get(), 0, p_width * p_height * 3 * sizeof(unsigned char));
    data = std::move(temp_data);
    this->width = p_width;
    this->height = p_height;
}

void Texture2DRGB::parse_ppm(const char* file_path) {
    std::string str_content = load_text_file(file_path);
    std::stringstream content(remove_ppm_comments(str_content));
    std::string line;
    // parse P3 ppm file
    content >> line;
    if (line != "P3") {
        throw std::runtime_error("Error: not a P3 ppm file");
    }
    content >> width >> height;
    double scaler;
    content >> scaler;
    scaler = 255.0 / scaler;
    DataType temp_data(new unsigned char[width * height * 3], free_data);
    int total = width * height * 3;
    // texture should go in reverse order
    for (int i = 0; i < width * height; ++i) {
        int r, g, b;
        content >> r >> g >> b;
        temp_data[total - 3 * i - 3] = r * scaler;
        temp_data[total - 3 * i - 2] = g * scaler;
        temp_data[total - 3 * i - 1] = b * scaler;
    }
    data = std::move(temp_data);
}

void Texture2DRGB::parse_img(const char* p_file_path) {
    if (get_ext(p_file_path) == "ppm") {
        parse_ppm(p_file_path);
        return;
    }
    DataType temp_data(stbi_load(to_unix_style_path(p_file_path).c_str(), &width, &height, nullptr, 3), (DataFreeFunc)stbi_image_free);
    if (temp_data == nullptr) {
        throw std::runtime_error("Failed to load image at path: " + to_unix_style_path(p_file_path));
        return;
    }
    data = std::move(temp_data);
}

unsigned int Texture2DRGB::get_texture_id() const {
    if (this->texture_id == 0) {
        GL_CALL(glGenTextures, 1, &this->texture_id);
        GL_CALL(glBindTexture, GL_TEXTURE_2D, this->texture_id);
        GL_CALL(glTexImage2D, GL_TEXTURE_2D, 0, config.srgb ? GL_SRGB : GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.get());
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

void Texture2DRGB::bind() const {
    GL_CALL(glBindTexture, GL_TEXTURE_2D, get_texture_id());
}

void Texture2DRGB::unbind() const {
    GL_CALL(glBindTexture, GL_TEXTURE_2D, 0);
}

void Texture2DRGB::set_texture_row(int row, const unsigned char* row_data) {
    if (row < 0 || row >= height) {
        std::stringstream ss;
        ss << "Row out of bounds: " << row;
        throw std::runtime_error(ss.str());
    }
    for (int i = 0; i < width * 3; ++i) {
        data[row * width * 3 + i] = row_data[i];
    }
    GL_CALL(glBindTexture, GL_TEXTURE_2D, get_texture_id());
    GL_CALL(glTexSubImage2D, GL_TEXTURE_2D, 0, 0, row, width, 1, GL_RGB, GL_UNSIGNED_BYTE, row_data);
    GL_CALL(glBindTexture, GL_TEXTURE_2D, 0);
}
