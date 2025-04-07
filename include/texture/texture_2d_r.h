#ifndef __TEXTURE_2D_F_H__
#define __TEXTURE_2D_F_H__
#include "texture/texture_2d.h"
#include <memory>
class Texture2DR : public Texture2D {
public:
    Texture2DR(const TextureConfigurations& p_config)
        : config(p_config), data(nullptr, free_data) {}
    virtual ~Texture2DR() {}
    Texture2DR(const Texture2DR&) = delete;

    // Create an empty texture
    virtual void create_empty_texture(int p_width, int p_height) override;

    // Used for assimp loaded model's textures
    virtual void parse_img(const char* P_file_path) override;

    // Bind the texture
    virtual void bind() const override;

    // Unbind the texture
    virtual void unbind() const override;

    // Get the texture row
    virtual void set_texture_row(int p_row, const unsigned char* p_row_data);
private:
    static void free_data(unsigned char* p) { delete[] p; }
    TextureConfigurations config;
    mutable unsigned int texture_id = 0;
    unsigned int get_texture_id() const;
    using DataFreeFunc = decltype(&free_data);
    using DataType = std::unique_ptr<unsigned char[], DataFreeFunc>;
    DataType data;
};
#endif
