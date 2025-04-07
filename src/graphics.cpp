#include "graphics.h"
#include "data/skeleton_model.h"
#include "utils.hpp"
#include "point_light.h"
#include "spot_light.h"
#include <glm/ext/scalar_constants.hpp>
#include <sstream>

GLuint load_shader(const char* shader_path, const std::string& shader_source, GLenum shader_type) {
    GLuint shader = GL_CALL(glCreateShader, shader_type);
    const char* shader_source_cstr = shader_source.c_str();
    GL_CALL(glShaderSource, shader, 1, &shader_source_cstr, nullptr);
    GL_CALL(glCompileShader, shader);
    GLint success;
    GL_CALL(glGetShaderiv, shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar info_log[1024];
        GL_CALL(glGetShaderInfoLog, shader, 1024, nullptr, info_log);
        std::stringstream ss;
        ss << "Error compiling shader " << shader_path << ": " << info_log << std::endl;
        throw std::runtime_error(ss.str());
    }
    return shader;
}

template<typename T>
void replace_str(std::string& str, const std::string& find, T&& replace) {
    auto pos = str.find(find);
    std::string replace_str = std::to_string(std::forward<T>(replace));
    while (pos != std::string::npos) {
        str.replace(pos, find.length(), replace_str);
        pos = str.find(find, pos + replace_str.length());
    }
}

void precompile_shader(std::string& p_shader_source) {
    replace_str(p_shader_source, "Material::MAX_TEXTURE_COUNT", Material::MAX_TEXTURE_COUNT);
    replace_str(p_shader_source, "PointLight::MAX_LIGHTS", PointLight::MAX_COUNT);
    replace_str(p_shader_source, "PointLight::SHADOW_MAP_BIAS", PointLight::SHADOW_MAP_BIAS);
    replace_str(p_shader_source, "PointLight::SHADOW_FILTER_SIZE", PointLight::SHADOW_FILTER_SIZE);
    replace_str(p_shader_source, "PointLight::SHADOW_FILTER_COUNT", PointLight::SHADOW_FILTER_COUNT);

    replace_str(p_shader_source, "SpotLight::MAX_LIGHTS", SpotLight::MAX_COUNT);
    replace_str(p_shader_source, "SpotLight::SHADOW_FILTER_SIZE", SpotLight::SHADOW_FILTER_SIZE);
    replace_str(p_shader_source, "SpotLight::RSM_SAMPLE_COUNT", SpotLight::RSM_SAMPLE_COUNT);
    replace_str(p_shader_source, "SpotLight::RSM_SAMPLE_RMAX", SpotLight::RSM_SAMPLE_RMAX);
    replace_str(p_shader_source, "SpotLight::GIType::NONE", SpotLight::GIType::NONE);
    replace_str(p_shader_source, "SpotLight::GIType::RSM", SpotLight::GIType::RSM);
    replace_str(p_shader_source, "SpotLight::GIType::LPV", SpotLight::GIType::LPV);

    replace_str(p_shader_source, "DEPTH_PREPASS_BIAS", 0.0001);
    replace_str(p_shader_source, "MAX_BONE_INFLUENCE", SkeletonModel::MAX_BONE_INFLUENCE);
    replace_str(p_shader_source, "MORPH_NUM_MAX", SkeletonModel::MorphAnimMesh::MORPH_NUM_MAX);
    replace_str(p_shader_source, "VERTEX_LOCATION_POS", VERTEX_LOCATION_POS);
    replace_str(p_shader_source, "VERTEX_LOCATION_NORMAL", VERTEX_LOCATION_NORMAL);
    replace_str(p_shader_source, "VERTEX_LOCATION_UV", VERTEX_LOCATION_UV);
    replace_str(p_shader_source, "VERTEX_LOCATION_TANGENT", VERTEX_LOCATION_TANGENT);
    replace_str(p_shader_source, "VERTEX_LOCATION_BITANGENT", VERTEX_LOCATION_BITANGENT);
    replace_str(p_shader_source, "VERTEX_LOCATION_BONE_ID", VERTEX_LOCATION_BONE_ID);
    replace_str(p_shader_source, "VERTEX_LOCATION_BONE_WEIGHT", VERTEX_LOCATION_BONE_WEIGHT);
    replace_str(p_shader_source, "PI", glm::pi<float>());
}

GLuint create_shader_program(const char* p_vert_shader_path, const char* p_frag_shader_path, const char* p_geom_shader_path) {
    REQUIRE_NON_NULL(p_vert_shader_path);
    REQUIRE_NON_NULL(p_frag_shader_path);
    std::string vert_shader_source = load_text_file(p_vert_shader_path);
    precompile_shader(vert_shader_source);
    GLuint vert_shader = load_shader(p_vert_shader_path, vert_shader_source, GL_VERTEX_SHADER);
    GLuint geom_shader = 0;
    if (p_geom_shader_path != nullptr) {
        std::string geom_shader_source = load_text_file(p_geom_shader_path);
        precompile_shader(geom_shader_source);
        geom_shader = load_shader(p_geom_shader_path, geom_shader_source, GL_GEOMETRY_SHADER);
    }
    std::string frag_shader_source = load_text_file(p_frag_shader_path);
    precompile_shader(frag_shader_source);
    GLuint frag_shader = load_shader(p_frag_shader_path, frag_shader_source, GL_FRAGMENT_SHADER);
    GLuint shader_program = GL_CALL(glCreateProgram);
    GL_CALL(glAttachShader, shader_program, vert_shader);
    if (p_geom_shader_path != nullptr) {
        GL_CALL(glAttachShader, shader_program, geom_shader);
    }
    GL_CALL(glAttachShader, shader_program, frag_shader);
    GL_CALL(glLinkProgram, shader_program);
    GLint success;
    GL_CALL(glGetProgramiv, shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        GLint log_length = 0;
        glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &log_length);
        std::unique_ptr<GLchar[]> info_log = std::unique_ptr<GLchar[]>(new GLchar[log_length]);
        GL_CALL(glGetProgramInfoLog, shader_program, 1024, nullptr, info_log.get());
        std::stringstream ss;
        ss << "Error linking shader program with:\nvertex shader " << p_vert_shader_path
           << "\nfragment shader " << p_frag_shader_path;
        if (p_geom_shader_path != nullptr) {
            ss << "\ngeometry shader " << p_geom_shader_path;
        }
        ss << ":\n";
        ss << info_log << std::endl;
        throw std::runtime_error(ss.str());
    }
    GL_CALL(glDetachShader, shader_program, vert_shader);
    if (p_geom_shader_path != nullptr) {
        GL_CALL(glDetachShader, shader_program, geom_shader);
    }
    GL_CALL(glDetachShader, shader_program, frag_shader);
    GL_CALL(glDeleteShader, vert_shader);
    if (p_geom_shader_path != nullptr) {
        GL_CALL(glDeleteShader, geom_shader);
    }
    GL_CALL(glDeleteShader, frag_shader);
    return shader_program;
}

GLuint create_compute_shader_program(const char* p_compute_shader_path) {
    REQUIRE_NON_NULL(p_compute_shader_path);
    std::string shader_source = load_text_file(p_compute_shader_path);
    precompile_shader(shader_source);
    GLuint shader = load_shader(p_compute_shader_path, shader_source, GL_COMPUTE_SHADER);
    GLuint shader_program = GL_CALL(glCreateProgram);
    GL_CALL(glAttachShader, shader_program, shader);
    GL_CALL(glLinkProgram, shader_program);
    GLint success;
    GL_CALL(glGetProgramiv, shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        GLint log_length = 0;
        glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &log_length);
        std::unique_ptr<GLchar[]> info_log = std::unique_ptr<GLchar[]>(new GLchar[log_length]);
        GL_CALL(glGetProgramInfoLog, shader_program, 1024, nullptr, info_log.get());
        std::stringstream ss;
        ss << "Error linking compute shader program: " << p_compute_shader_path << ":\n";
        ss << info_log << std::endl;
        throw std::runtime_error(ss.str());
    }
    GL_CALL(glDetachShader, shader_program, shader);
    GL_CALL(glDeleteShader, shader);
    return shader_program;
}

GLint get_max_compute_workgroup_size(unsigned int p_group) {
    assert(p_group < 3);
    GLint max_workgroup_size;
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, p_group, &max_workgroup_size);
    return max_workgroup_size;
}
