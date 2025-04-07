#include "shader/shader_compute_deserializer.h"
#include "resource/resource_unit.h"
#include "system/system.h"

UniversalResourceUnit* ShaderComputeDeserializer::deserialize(const std::string& p_id, const toml::table& p_header_table) {
    auto section = p_header_table[p_id];
    std::string path = toml_get_required_value<std::string>(section.as_table(), "path");
    path = System::get_singleton()->get_exe_directory() + "/" + removed_quotes(path);
    return new UniversalResourceUnit(p_id, std::make_shared<ShaderCompute>(path), get_type());
}
