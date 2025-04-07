#include "shader/shader_pipeline_deserializer.h"
#include "shader/shader_pipeline.h"
#include "system/system.h"
#include "utils.hpp"

UniversalResourceUnit* ShaderPipelineDeserializer::deserialize(const std::string& p_id, const toml::table& p_header_table) {
    auto section = p_header_table[p_id];
    std::string vertex_path = toml_get_required_value<std::string>(section.as_table(), "vertex");
    std::string fragment_path = toml_get_required_value<std::string>(section.as_table(), "fragment");
    std::optional<std::string> geometry_path = toml_get_optional_value<std::string>(section.as_table(), "geometry");
    std::string exe_directory = System::get_singleton()->get_exe_directory();
    vertex_path = exe_directory + "/" + removed_quotes(vertex_path);
    fragment_path = exe_directory + "/" + removed_quotes(fragment_path);
    std::shared_ptr<ShaderPipeline> shader_pipeline;
    if (geometry_path.has_value()) {
        *geometry_path = exe_directory + "/" + removed_quotes(*geometry_path);
        shader_pipeline = std::make_shared<ShaderPipeline>(vertex_path, fragment_path, *geometry_path);
    }
    else {
        shader_pipeline = std::make_shared<ShaderPipeline>(vertex_path, fragment_path);
    }
    return new UniversalResourceUnit(p_id, shader_pipeline, get_type());
}
