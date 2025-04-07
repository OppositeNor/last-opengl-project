#include "texture/texture_2d_rgb_deserializer.h"
#include "system/system.h"
#include "texture/texture.hpp"
#include "texture/texture_2d_rgb.h"

UniversalResourceUnit* Texture2DRGBDeserializer::deserialize(const std::string& p_id, const toml::table& p_header_table) {
    auto section = p_header_table[p_id];
    std::string path = removed_quotes(toml_get_required_value<std::string>(section.as_table(), "path"));
    path = System::get_singleton()->get_exe_directory() + "/" + path;
    TextureConfigurations config;
    config.scale_nearest_min = toml_get_optional_value<bool>(section.as_table(), "scale_nearest_min", true);
    config.scale_nearest_mag = toml_get_optional_value<bool>(section.as_table(), "scale_nearest_mag", true);
    config.generate_mipmap = toml_get_optional_value<bool>(section.as_table(), "generate_mipmap", false);
    std::shared_ptr<Texture2DRGB> texture = std::make_shared<Texture2DRGB>(config, path);
    return new UniversalResourceUnit(p_id, std::shared_ptr<Texture2DRGB>(std::move(texture)), get_type());
}
