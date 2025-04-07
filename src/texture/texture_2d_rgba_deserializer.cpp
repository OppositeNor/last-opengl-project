#include "texture/texture_2d_rgba_deserializer.h"
#include "system/system.h"

UniversalResourceUnit* Texture2DRGBADeserializer::deserialize(const std::string& p_id, const toml::table& p_header_table) {
    auto section = p_header_table[p_id];
    std::string path = removed_quotes(toml_get_required_value<std::string>(section.as_table(), "path"));
    path = System::get_singleton()->get_exe_directory() + "/" + path;
    TextureConfigurations config;
    config.scale_nearest_min = toml_get_optional_value<bool>(section.as_table(), "scale_nearest_min", true);
    config.scale_nearest_mag = toml_get_optional_value<bool>(section.as_table(), "scale_nearest_mag", true);
    config.generate_mipmap = toml_get_optional_value<bool>(section.as_table(), "generate_mipmap", false);
    std::shared_ptr<Texture2DRGBA> texture = std::make_shared<Texture2DRGBA>(config, path);
    return new UniversalResourceUnit(p_id, std::shared_ptr<Texture2DRGBA>(std::move(texture)), get_type());
}
