#include "texture/texture_2d.h"
#include "graphics.h"
#include <utility>
#include <stdexcept>

void Texture2D::set_wrap(TextureConfigurations::WrapMode p_wrap_mode, unsigned int p_wrap_dimension) {
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

unsigned char* get_pixel(unsigned char* p_data, int p_row, int p_col, int p_width, int p_channel) {
    return p_data + (p_row * p_width + p_col) * p_channel;
}

void Texture2D::rotate_image_data_cw(unsigned char* p_data, int p_width, int p_height, int p_channel) {
    if (p_width != p_height) {
        throw std::runtime_error("Only allowed to rotate a square image.");
    }
    // transpose
    for (int i = 0; i < p_height; ++i) {
        for (int j = i; j < p_width; ++j) {
            for (int k = 0; k < p_channel; ++k) {
                std::swap(get_pixel(p_data, i, j, p_width, p_channel)[k], get_pixel(p_data, j, i, p_width, p_channel)[k]);
            }
        }
    }
    int half_width = p_width >> 1;
    // reverse
    for (int i = 0; i < p_height; ++i) {
        for (int j = 0; j < half_width; ++j) {
            for (int k = 0; k < p_channel; ++k) {
                std::swap(get_pixel(p_data, i, j, p_width, p_channel)[k],
                          get_pixel(p_data, i, p_width - j - 1, p_width, p_channel)[k]);
            }
        }
    }
}

void Texture2D::rotate_image_data_ccw(unsigned char* p_data, int p_width, int p_height, int p_channel) {
    if (p_width != p_height) {
        throw std::runtime_error("Only allowed to rotate a square image.");
    }
    // transpose
    for (int i = 0; i < p_height; ++i) {
        for (int j = i; j < p_width; ++j) {
            for (int k = 0; k < p_channel; ++k) {
                std::swap(get_pixel(p_data, i, j, p_width, p_channel)[k], get_pixel(p_data, j, i, p_width, p_channel)[k]);
            }
        }
    }
    int half_height = p_height >> 1;
    // reverse
    for (int i = 0; i < half_height; ++i) {
        for (int j = 0; j < p_width; ++j) {
            for (int k = 0; k < p_channel; ++k) {
                std::swap(get_pixel(p_data, i, j, p_width, p_channel)[k],
                          get_pixel(p_data, p_height - i - 1, j, p_width, p_channel)[k]);
            }
        }
    }
}

void Texture2D::rotate_image_data_180(unsigned char* p_data, int p_width, int p_height, int p_channel) {
    int image_size = p_width * p_height;
    int half_img_size = image_size << 1;
    for (int i = 0; i < half_img_size; ++i) {
        for (int j = 0; j < p_channel; ++j) {
            std::swap(p_data[i * p_channel + j], p_data[(image_size - i - 1) * p_channel + j]);
        }
    }
}
