#include "texture/texture_cube_deserializer.h"
#include "system/system.h"
#include "texture/texture.hpp"
#include "texture/texture_cube.h"

TextureConfigurations::RotateMode get_rotate_mode_from_str(const std::string& p_string) {
    if (p_string == "none") {
        return TextureConfigurations::RotateMode::NONE;
    }
    if (p_string == "cw") {
        return TextureConfigurations::RotateMode::CW;
    }
    if (p_string == "ccw") {
        return TextureConfigurations::RotateMode::CCW;
    }
    if (p_string == "180") {
        return TextureConfigurations::RotateMode::ONE_EIGHTY;
    }
    throw std::runtime_error("Unrecognized rotate direction: " + p_string);
}

UniversalResourceUnit* TextureCubeDeserializer::deserialize(const std::string& p_id, const toml::table& p_header_table) {
    auto section = p_header_table[p_id];
    TextureConfigurations config;
    config.scale_nearest_min = toml_get_optional_value<bool>(section.as_table(), "scale_nearest_min", true);
    config.scale_nearest_mag = toml_get_optional_value<bool>(section.as_table(), "scale_nearest_mag", true);
    config.generate_mipmap = toml_get_optional_value<bool>(section.as_table(), "generate_mipmap", false);
    std::optional<std::string> path = toml_get_optional_value<std::string>(section.as_table(), "path");
    std::string exe_dir = System::get_singleton()->get_exe_directory();
    if (path.has_value()) {
        path = removed_quotes(path.value());
        path = exe_dir + "/" + path.value();
        std::shared_ptr<TextureCube> texture = std::make_shared<TextureCube>(*path, config);
        return new UniversalResourceUnit(p_id, std::shared_ptr<TextureCube>(std::move(texture)), get_type());
    }
    config.rotate_config = {
        get_rotate_mode_from_str(toml_get_optional_value<std::string>(section.as_table(), "rotate_front", "none")),
        get_rotate_mode_from_str(toml_get_optional_value<std::string>(section.as_table(), "rotate_back", "none")),
        get_rotate_mode_from_str(toml_get_optional_value<std::string>(section.as_table(), "rotate_top", "none")),
        get_rotate_mode_from_str(toml_get_optional_value<std::string>(section.as_table(), "rotate_bottom", "none")),
        get_rotate_mode_from_str(toml_get_optional_value<std::string>(section.as_table(), "rotate_left", "none")),
        get_rotate_mode_from_str(toml_get_optional_value<std::string>(section.as_table(), "rotate_right", "none"))
    };
    std::array<std::string, 6> paths = {
        exe_dir + "/" + removed_quotes(toml_get_required_value<std::string>(section.as_table(), "front")),
        exe_dir + "/" + removed_quotes(toml_get_required_value<std::string>(section.as_table(), "back")),
        exe_dir + "/" + removed_quotes(toml_get_required_value<std::string>(section.as_table(), "top")),
        exe_dir + "/" + removed_quotes(toml_get_required_value<std::string>(section.as_table(), "bottom")),
        exe_dir + "/" + removed_quotes(toml_get_required_value<std::string>(section.as_table(), "left")),
        exe_dir + "/" + removed_quotes(toml_get_required_value<std::string>(section.as_table(), "right")),
    };
    return new UniversalResourceUnit(p_id, std::make_shared<TextureCube>(paths, false, config), get_type());
}
