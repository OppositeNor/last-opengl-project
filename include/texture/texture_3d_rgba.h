#ifndef __TEXTURE_3D_RGBA_H__
#define __TEXTURE_3D_RGBA_H__
#include "texture.hpp"
#include "texture/texture_3d.h"

class Texture3DRGBA : public Texture3D {
public:
    Texture3DRGBA(unsigned int p_width, unsigned int p_height, unsigned int p_depth, const TextureConfigurations& p_config);
    virtual ~Texture3DRGBA();

    // Create an empty texture
    virtual void create_empty_texture(int p_width, int p_height, int p_depth) override;

    // Bind the texture
    virtual void bind() const override;

    // Unbind the texture
    virtual void unbind() const override;

    // Bind to compute shader
    virtual void bind_compute(unsigned int p_unit, int p_mipmap_level) const override;

    // Unbind to compute shader
    virtual void unbind_compute(unsigned int p_unit) const override;

protected:
    unsigned int id;
    TextureConfigurations config;
};
#endif
