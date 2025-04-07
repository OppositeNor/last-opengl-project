#ifndef __TEXTURE_3D_H__
#define __TEXTURE_3D_H__
#include "texture/texture.hpp"
class Texture3D {
public:

    Texture3D() {}
    virtual ~Texture3D() {}

    // Create an empty texture
    virtual void create_empty_texture(int p_width, int p_height, int p_depth) = 0;

    // Bind the texture
    virtual void bind() const = 0;

    // Unbind the texture
    virtual void unbind() const = 0;

    // Bind to compute shader
    virtual void bind_compute(unsigned int p_unit, int p_mipmap_level) const = 0;

    // Unbind to a compute shader
    virtual void unbind_compute(unsigned int p_unit) const = 0;

    bool is_cube() const {
        return width == height && height == depth;
    }

    // Get the width of the texture
    unsigned int get_width() const {
        return width;
    }

    // Get the height of the texture
    unsigned int get_height() const {
        return height;
    }

    // Get the depth of the texture
    unsigned int get_depth() const {
        return depth;
    }

protected:
    unsigned int width, height, depth;

    static void set_wrap(TextureConfigurations::WrapMode p_wrap_mode, unsigned int p_wrap_dimension);
};
#endif
