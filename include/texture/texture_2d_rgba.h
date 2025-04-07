#ifndef __TEXTURE_2D_RGBA_H__
#define __TEXTURE_2D_RGBA_H__
#include "resource/resource_base.h"
#include "texture_2d.h"
#include <memory>

template<>
struct get_res_type<class Texture2DRGBA> {
    constexpr static const char* VALUE = "texture_2d_rgba";
};

class Texture2DRGBA : public Texture2D, public ResourceBase {
public:
    Texture2DRGBA(const TextureConfigurations& p_config, const std::string& p_file_path = "")
        : file_path(p_file_path), config(p_config), texture_id(0), data(nullptr, free_data) {}
    virtual ~Texture2DRGBA();
    Texture2DRGBA(const Texture2DRGBA&) = delete;

    virtual void load() override;

    // Create an empty texture
    virtual void create_empty_texture(int p_width, int p_height) override;

    // Used for assimp loaded model's textures
    virtual void parse_img(const char* p_file_path) override;

    // Bind the texture
    virtual void bind() const override;

    // Unbind the texture
    virtual void unbind() const override;

    // Get the texture row
    virtual void set_texture_row(int p_row, const unsigned char* p_row_data);

protected:
    static void free_data(unsigned char* p) { delete[] p; }
    std::string file_path;
    TextureConfigurations config;
    mutable unsigned int texture_id = 0;
    unsigned int get_texture_id() const;
    using DataFreeFunc = decltype(&free_data);
    using DataType = std::unique_ptr<unsigned char[], DataFreeFunc>;
    DataType data;
};

#endif
