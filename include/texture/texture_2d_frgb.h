#ifndef __TEXTURE_2D_FRGB_H__
#define __TEXTURE_2D_FRGB_H__

#include "texture/texture_2d.h"
#include <memory>

class Texture2DFRGB : public Texture2D {
public:
    Texture2DFRGB(const TextureConfigurations& p_config)
        : texture_id(0), config(p_config), data(nullptr, free_data) {};
    virtual ~Texture2DFRGB();

    // Create an empty texture
    virtual void create_empty_texture(int p_width, int p_height) override;

    // Used for assimp loaded model's textures
    virtual void parse_img(const char* p_file_path) override;

    // Bind the texture
    virtual void bind() const override;

    // Unbind the texture
    virtual void unbind() const override;

    // Get the texture row
    virtual void set_texture_row(int p_row, const float* p_row_data);

    // Get the texture row
    virtual void get_texture_row(int p_row, float* p_buffer) const;

private:
    static void free_data(float* p) { delete[] p; }
    mutable unsigned int texture_id;
    unsigned int get_texture_id() const;
    TextureConfigurations config;
    using DataFreeFunc = decltype(&free_data);
    using DataType = std::unique_ptr<float[], DataFreeFunc>;
    DataType data;
};

#endif
