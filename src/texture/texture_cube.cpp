#include "texture/texture_cube.h"
#include "texture/texture.hpp"
#include "utils.hpp"
#include "graphics.h"
#include <stb_image.h>
#include <sstream>

TextureCube::TextureCube(const std::string& p_path, const TextureConfigurations& p_config)
    : config(p_config), data{DataType(nullptr, free_data), DataType(nullptr, free_data),
                             DataType(nullptr, free_data), DataType(nullptr, free_data),
                             DataType(nullptr, free_data), DataType(nullptr, free_data)} {
    width = 0;
    height = 0;
    texture_id = 0;
    paths = std::array<std::string, 6>({p_path, "", "", "", "", ""});
    is_single = true;
}

TextureCube::TextureCube(const std::array<std::string, 6>& p_paths, bool p_is_single, const TextureConfigurations& p_config)
    : config(p_config), data{DataType(nullptr, free_data), DataType(nullptr, free_data),
                             DataType(nullptr, free_data), DataType(nullptr, free_data),
                             DataType(nullptr, free_data), DataType(nullptr, free_data)} {
    width = 0;
    height = 0;
    texture_id = 0;
    paths = p_paths;
    is_single = p_is_single;
}

TextureCube::~TextureCube() {
    if (texture_id != 0) {
        GL_CALL(glDeleteTextures, 1, &texture_id);
    }
}

void TextureCube::create_empty_texture(int p_width, int p_height) {
    if (p_width != p_height) {
        throw std::runtime_error("Error: the texture should be a square");
    }
    DataType data_temp(new unsigned char[p_width * p_height * 3 * 6], free_data);
    for (int i = 0; i < p_width * p_height * 3 * 6; ++i) {
        data_temp[i] = 0;
    }
    width = p_width;
    height = p_height;
    data[0] = std::move(data_temp);
}

void TextureCube::parse_img(const char* p_file_path) {
    data[0] = get_image_data(width, height, p_file_path); // TODO
    switch (config.rotate_config[0]) {
        case TextureConfigurations::RotateMode::NONE:
            break;
        case TextureConfigurations::RotateMode::CW:
            rotate_image_data_cw(data[0].get(), width, height, 3);
            break;
        case TextureConfigurations::RotateMode::CCW:
            rotate_image_data_ccw(data[0].get(), width, height, 3);
            break;
        case TextureConfigurations::RotateMode::ONE_EIGHTY:
            rotate_image_data_180(data[0].get(), width, height, 3);
            break;
    };
    for (int i = 1; i < 6; ++i) {
        data[i].reset();
    }
}

TextureCube::DataType TextureCube::get_image_data(int& p_width, int& p_height, const char* p_path) {
    if (get_ext(p_path) == "ppm") {
        return get_ppm_image_data(p_width, p_height, p_path);
    }
    DataType temp_data(stbi_load(to_unix_style_path(p_path).c_str(), &p_width, &p_height, nullptr, 3), (DataFreeFunc)stbi_image_free);
    if (temp_data == nullptr) {
        throw std::runtime_error("Failed to load image at path: " + to_unix_style_path(p_path));
    }
    return temp_data;
}

TextureCube::DataType TextureCube::get_ppm_image_data(int& p_width, int& p_height, const char* p_path) {
    std::string str_content = load_text_file(p_path);
    std::stringstream content(remove_ppm_comments(str_content));
    std::string line;
    // parse P3 ppm file
    content >> line;
    if (line != "P3") {
        throw std::runtime_error("Error: not a P3 ppm file");
    }
    int temp_width, temp_height;
    content >> temp_width >> temp_height;
    if (temp_width != temp_height) {
        throw std::runtime_error("Error: the texture should be a square");
    }
    if (p_width != 0 && temp_width != p_width) {
        throw std::runtime_error("Error: all the faces of the cube should have the same dimension");
    }
    double scaler;
    content >> scaler;
    scaler = 255.0 / scaler;
    DataType temp_data(new unsigned char[temp_width * temp_height * 3], free_data);
    p_width = temp_width;
    p_height = temp_height;
    int total = temp_width * temp_height;
    for (int i = 0; i < total; ++i) {
        int r, g, b;
        content >> r >> g >> b;
        int first = i * 3;
        temp_data[first + 0] = r * scaler;
        temp_data[first + 1] = g * scaler;
        temp_data[first + 2] = b * scaler;
    }
    return temp_data;
}

void TextureCube::parse_img(const char* p_path_front, const char* p_path_back,
                            const char* p_path_top, const char* p_path_bottom,
                            const char* p_path_left, const char* p_path_right) {
    const char* paths[] = {
        p_path_front, p_path_back,
        p_path_top, p_path_bottom,
        p_path_left, p_path_right
    };

    for (int i = 0; i < 6; ++i) {
        data[i] = get_image_data(width, height, paths[i]);
        switch (config.rotate_config[i]) {
            case TextureConfigurations::RotateMode::NONE:
                break;
            case TextureConfigurations::RotateMode::CW:
                rotate_image_data_cw(data[i].get(), width, height, 3);
                break;
            case TextureConfigurations::RotateMode::CCW:
                rotate_image_data_ccw(data[i].get(), width, height, 3);
                break;
            case TextureConfigurations::RotateMode::ONE_EIGHTY:
                rotate_image_data_180(data[i].get(), width, height, 3);
                break;
        };

    }
}

void TextureCube::load() {
    if (is_single) {
        parse_img(paths[0].c_str());
    }
    else {
        parse_img(paths[0].c_str(),
                  paths[1].c_str(),
                  paths[2].c_str(),
                  paths[3].c_str(),
                  paths[4].c_str(),
                  paths[5].c_str());
    }
}

unsigned int TextureCube::get_texture_id() const {
    if (this->texture_id == 0) {
        GL_CALL(glGenTextures, 1, &this->texture_id);
        GL_CALL(glBindTexture, GL_TEXTURE_CUBE_MAP, this->texture_id);
        for (unsigned int i = 0; i < 6; ++i) {
            if (data[i] == nullptr) {
                GL_CALL(glTexImage2D, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, config.srgb ? GL_SRGB : GL_RGB,
                        width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data[0].get());
            }
            else {
                GL_CALL(glTexImage2D, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, config.srgb ? GL_SRGB : GL_RGB,
                        width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data[i].get());
            }
        }
        GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, config.scale_nearest_min ? GL_NEAREST : GL_LINEAR);
        GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, config.scale_nearest_mag ? GL_NEAREST : GL_LINEAR);
        set_wrap(config.wrap_mode_s, GL_TEXTURE_WRAP_S);
        set_wrap(config.wrap_mode_t, GL_TEXTURE_WRAP_T);
        set_wrap(config.wrap_mode_r, GL_TEXTURE_WRAP_R);
        GL_CALL(glBindTexture, GL_TEXTURE_CUBE_MAP, 0);
    }
    return this->texture_id;
}

void TextureCube::bind() const {
    GL_CALL(glBindTexture, GL_TEXTURE_CUBE_MAP, get_texture_id());
}

void TextureCube::unbind() const {
    GL_CALL(glBindTexture, GL_TEXTURE_CUBE_MAP, 0);
}

