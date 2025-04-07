#include "data/obj_model_deserializer.h"
#include "system/system.h"

UniversalResourceUnit* OBJModelDeserializer::deserialize(const std::string& p_id, const toml::table& p_header_table) {
    auto section = p_header_table[p_id];
    std::string path = removed_quotes(toml_get_required_value<std::string>(section.as_table(), "path"));
    path = System::get_singleton()->get_exe_directory() + "/" + path;
    std::shared_ptr<OBJModel> model = std::make_shared<OBJModel>(get_obj_id(), path);
    return new UniversalResourceUnit(p_id, model, get_type());
}
