#ifndef __GRAPHICS_HEADER_H__
#define __GRAPHICS_HEADER_H__

#include <glad/glad.h>
#include <string>

#ifdef NDEBUG
#define GL_CALL(func, ...) func(__VA_ARGS__)
#else
#include <sstream>
#define GL_CALL(func, ...) func(__VA_ARGS__); CheckGLError(__FILE__, __LINE__)

inline void CheckGLError(const char* file, int line) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::stringstream ss;
        ss << "OpenGL error: " << std::hex << error << std::dec << " at " << file << ":" << line << std::endl;
        throw std::runtime_error(ss.str());
    }
}
#endif

// shaders
GLuint load_shader(const char* p_shader_path, const std::string& p_shader_source, GLenum p_shader_type);
GLuint create_shader_program(const char* p_vert_shader_path, const char* p_frag_shader_path, const char* p_geom_shader_path = nullptr);
GLuint create_compute_shader_program(const char* p_compute_shader_path);

GLint get_max_compute_workgroup_size(unsigned int p_group);

#endif
