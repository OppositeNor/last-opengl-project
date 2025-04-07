#ifndef __TEXTURE_2D_RGB_H__
#define __TEXTURE_2D_RGB_H__
#include "resource/resource_base.h"
#include "texture/texture_2d.h"
#include <cstdlib>
#include <memory>
#include <string>

template<>
struct get_res_type<class Texture2DRGB> {
    constexpr static const char* VALUE = "texture_2d_rgb";
};

class Texture2DRGB : public Texture2D, public ResourceBase {
public:
    Texture2DRGB(const TextureConfigurations& p_config, const std::string& p_path = "");
    virtual ~Texture2DRGB() override;
    Texture2DRGB(const Texture2DRGB&) = delete;

    virtual void load() override;

    // Create an empty texture
    virtual void create_empty_texture(int width, int height) override;

    // Used for assimp loaded model's textures
    virtual void parse_img(const char* file_path) override;

    // Bind the texture
    virtual void bind() const override;

    // Unbind the texture
    virtual void unbind() const override;

    // Get the texture row
    virtual void set_texture_row(int row, const unsigned char* row_data);
private:
    static void free_data(unsigned char* p) { delete[] p; }
    mutable unsigned int texture_id;
    unsigned int get_texture_id() const;
    std::string path;
    TextureConfigurations config;
    using DataFreeFunc = decltype(&free_data);
    using DataType = std::unique_ptr<unsigned char[], DataFreeFunc>;
    DataType data;
    void parse_ppm(const char* p_path);
    void push_data_vram(const DataType& p_data) const;
};

#endif
