#ifndef __TEXTURE_2D_RGB_GPU_H__
#define __TEXTURE_2D_RGB_GPU_H__

#include "texture/texture_2d_gpu.h"

class Texture2DRGBGPU : public Texture2DGPU {
public:
    Texture2DRGBGPU(const TextureConfigurations& p_config)
        : texture_id(0), config(p_config) {}

    virtual ~Texture2DRGBGPU() override;

    // Create an empty texture
    virtual void create_empty_texture(int p_width, int p_height) override;

    // Used for assimp loaded model's textures
    virtual void parse_img(const char* p_file_path) override;

    // Bind the texture
    virtual void bind() const override;

    // Unbind the texture
    virtual void unbind() const override;

private:
    unsigned int texture_id;
    unsigned int get_texture_id();
    void set_gpu_data(unsigned char* p_data);
    TextureConfigurations config;
};

#endif
