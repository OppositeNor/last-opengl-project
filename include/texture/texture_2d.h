#ifndef __TEXTURE_2D_H__
#define __TEXTURE_2D_H__
#include "texture.hpp"

class Texture2D {
public:
    Texture2D() {}
    virtual ~Texture2D() {}
    Texture2D(const Texture2D&) = delete;

    // Create an empty texture
    virtual void create_empty_texture(int p_width, int p_height) = 0;

    // Used for assimp loaded model's textures
    virtual void parse_img(const char* p_file_path) = 0;

    // Bind the texture
    virtual void bind() const = 0;

    // Unbind the texture
    virtual void unbind() const = 0;

    // Get the width of the texture
    int get_width() const {
        return width;
    }

    // Get the height of the texture
    int get_height() const {
        return height;
    }

protected:
    int width;
    int height;

    static void set_wrap(TextureConfigurations::WrapMode p_wrap_mode, unsigned int p_wrap_dimension);
    static void rotate_image_data_cw(unsigned char* p_data, int p_data_width, int p_data_height, int p_data_channel);
    static void rotate_image_data_ccw(unsigned char* p_data, int p_data_width, int p_data_height, int p_data_channel);
    static void rotate_image_data_180(unsigned char* p_data, int p_data_width, int p_data_height, int p_data_channel);
};

#endif
