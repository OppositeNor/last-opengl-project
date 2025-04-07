#include "shader/shader_pipeline.h"
#include "graphics.h"

ShaderPipeline::ShaderPipeline(const std::string& p_vertex_shader_path,
                               const std::string& p_fragment_shader_path,
                               const std::string& p_geometry_shader_path)
    : vertex_shader_path(p_vertex_shader_path),
    fragment_shader_path(p_fragment_shader_path),
    geometry_shader_path(p_geometry_shader_path) {
    set_is_main_only(true);
}

void ShaderPipeline::load() {
    setup();
}

ShaderPipeline::~ShaderPipeline() {
    if (id != 0) {
        GL_CALL(glDeleteProgram, id);
        id = 0;
    }
}

void ShaderPipeline::setup() {
    if (id != 0) {
        GL_CALL(glDeleteProgram, id);
    }
    if (geometry_shader_path == "") {
        id = create_shader_program(vertex_shader_path.c_str(), fragment_shader_path.c_str(), nullptr);
    } else {
        id = create_shader_program(vertex_shader_path.c_str(), fragment_shader_path.c_str(), geometry_shader_path.c_str());
    }
}

