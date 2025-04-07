#include "data/skeletal_animation_deserializer.h"
#include "system/system.h"

UniversalResourceUnit* SkeletalAnimationDataDeserializer::deserialize(const std::string& p_id, const toml::table& p_header_table) {
    auto section = p_header_table[p_id];
    std::string path = removed_quotes(toml_get_required_value<std::string>(section.as_table(), "path"));
    path = System::get_singleton()->get_exe_directory() + "/" + path;
    bool mesh_z_up_to_y_up = toml_get_optional_value(section.as_table(), "mesh_z_up_to_y_up", false);
    std::optional animations = section["animations"].as_array();
    SkeletalAnimationData::SkeletonAnimationDataInfo info;
    if (animations.has_value() && *animations != nullptr) {
        std::vector<std::string> animations_paths;
        animations_paths.reserve((*animations)->size());
        for (auto& anim : **animations) {
            std::optional anim_path = anim.value<std::string>();
            if (!anim_path.has_value()) {
                throw std::runtime_error("Animation must be a string.");
            }
            animations_paths.push_back(System::get_singleton()->get_exe_directory() + "/" + removed_quotes(*anim_path));
        }
        info.is_separate = true;
        info.path = path;
        info.animations_paths = animations_paths;
        info.mesh_z_up_to_y_up = mesh_z_up_to_y_up;
    }
    else {
        info.is_separate = false;
        info.path = path;
        info.mesh_z_up_to_y_up = mesh_z_up_to_y_up;
    }
    std::shared_ptr<SkeletalAnimationData> skeletal_animation_data = std::make_shared<SkeletalAnimationData>(get_obj_id(), info);
    return new UniversalResourceUnit(p_id, skeletal_animation_data, get_type());
}
