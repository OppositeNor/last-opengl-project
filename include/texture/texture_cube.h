#ifndef __TEXTURE_CUBE_H__
#define __TEXTURE_CUBE_H__
#include "resource/resource_base.h"
#include "texture/texture_2d.h"
#include <array>
#include <memory>

template<>
struct get_res_type<class TextureCube> {
    constexpr static const char* VALUE = "texture_cube";
};

class TextureCube : public Texture2D, public ResourceBase {
public:
    TextureCube(const std::string& p_path, const TextureConfigurations& p_config);
    TextureCube(const std::array<std::string, 6>& p_paths, bool p_is_single, const TextureConfigurations& p_config);
    virtual ~TextureCube() override;

    // Create an empty texture
    virtual void create_empty_texture(int p_width, int p_height) override;

    virtual void parse_img(const char* p_path) override;

    virtual void parse_img(const char* p_path_front, const char* p_path_back,
                           const char* p_path_top, const char* p_path_bottom,
                           const char* p_path_left, const char* p_path_right);

    virtual void load() override;

    // Bind the texture
    virtual void bind() const override;

    // Unbind the texture
    virtual void unbind() const override;

private:
    mutable unsigned int texture_id;
    unsigned int get_texture_id() const;
    TextureConfigurations config;
    static void free_data(unsigned char* p) { delete[] p; }
    using DataFreeFunc = decltype(&free_data);
    using DataType = std::unique_ptr<unsigned char[], DataFreeFunc>;
    std::array<DataType, 6> data;
    std::array<std::string, 6> paths;
    bool is_single;

    DataType get_image_data(int& p_width, int& p_height, const char* p_path);
    DataType get_ppm_image_data(int& p_width, int& p_height, const char* p_path);
};


#endif
