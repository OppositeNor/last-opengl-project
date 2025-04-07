#include "shader/shader_compute.h"
#include "graphics.h"

ShaderCompute::ShaderCompute(const std::string& p_path)
    : path(p_path){
    set_is_main_only(true);
}

ShaderCompute::~ShaderCompute() {}

void ShaderCompute::setup(const std::string& p_path) {
    if (id != 0) {
        GL_CALL(glDeleteProgram, id);
    }
    id = create_compute_shader_program(p_path.c_str());
}

void ShaderCompute::load() {
    setup(path);
}

void ShaderCompute::compute(unsigned int p_num_groups_x,
                            unsigned int p_num_groups_y,
                            unsigned int p_num_groups_z) const {
    GL_CALL(glDispatchCompute, p_num_groups_x, p_num_groups_y, p_num_groups_z);
}

void ShaderCompute::set_shader_storage_barrier() {
    GL_CALL(glMemoryBarrier, GL_SHADER_STORAGE_BARRIER_BIT);
}

void ShaderCompute::set_buffer_update_barrier() {
    GL_CALL(glMemoryBarrier, GL_BUFFER_UPDATE_BARRIER_BIT);
}

void ShaderCompute::set_va_barrier() {
    GL_CALL(glMemoryBarrier, GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
}

void ShaderCompute::set_all_barrier() {
    GL_CALL(glMemoryBarrier, GL_ALL_BARRIER_BITS);
}

void ShaderCompute::use() const {
    GL_CALL(glUseProgram, id);
}

void ShaderCompute::unuse() const {
    GL_CALL(glUseProgram, 0);
}
