#include "graphics.h"
#include "resource/global_resource.h"
#include "texture/texture_2d_rgb.h"
#include "texture/texture_3d.h"
#include "texture/texture_cube.h"
#include "utils.hpp"
#include "texture/texture_2d.h"
#include "point_light.h"
#include "spot_light.h"
#include "framebuffer/framebuffer.h"
#include <stdexcept>

Shader::~Shader() {
    if (id != 0) {
        GL_CALL(glDeleteProgram, id);
        id = 0;
    }
}

void Shader::use() const {
    GL_CALL(glUseProgram, id);
}

void Shader::reset() const {
    indices.clear();
}

void Shader::unuse() const {
    GL_CALL(glUseProgram, 0);
}

void Shader::set_uniform(const std::string& p_name, const glm::mat4& p_value, bool p_is_required) const {
    use();
    GLint location = GL_CALL(glGetUniformLocation, id, p_name.c_str());
    if (location == -1) {
        if (p_is_required) {
            std::cout << "Uniform " << p_name << " not found or not used in shader program " << id << '.' << std::endl;
        }
        return;
    }
    GL_CALL(glUniformMatrix4fv, location, 1, GL_FALSE, &p_value[0][0]);
}

void Shader::set_uniform(const std::string& p_name, const glm::vec3& p_value, bool p_is_required) const {
    use();
    GLint location = GL_CALL(glGetUniformLocation, id, p_name.c_str());
    if (location == -1) {
        if (p_is_required) {
            std::cout << "Uniform " << p_name << " not found or not used in shader program " << id << '.' << std::endl;
        }
        return;
    }
    GL_CALL(glUniform3fv, location, 1, &p_value[0]);
}

void Shader::set_uniform(const std::string& p_name, const Tuple<float, 3>& p_value, bool p_is_required) const {
    use();
    GLint location = GL_CALL(glGetUniformLocation, id, p_name.c_str());
    if (location == -1) {
        if (p_is_required) {
            std::cout << "Uniform " << p_name << " not found or not used in shader program " << id << '.' << std::endl;
        }
        return;
    }
    GL_CALL(glUniform3fv, location, 1, p_value.data);
}

void Shader::set_uniform(const std::string& p_name, int p_value, bool p_is_required) const {
    use();
    GLint location = GL_CALL(glGetUniformLocation, id, p_name.c_str());
    if (location == -1) {
        if (p_is_required) {
            std::cout << "Uniform " << p_name << " not found or not used in shader program " << id << '.' << std::endl;
        }
        return;
    }
    GL_CALL(glUniform1i, location, p_value);
}

void Shader::set_uniform(const std::string& p_name, float p_value, bool p_is_required) const {
    use();
    GLint location = GL_CALL(glGetUniformLocation, id, p_name.c_str());
    if (location == -1) {
        if (p_is_required) {
            std::cout << "Uniform " << p_name << " not found or not used in shader program " << id << '.' << std::endl;
        }
        return;
    }
    GL_CALL(glUniform1f, location, p_value);
}

void Shader::set_uniform(const std::string& p_name, float p_values[], int p_values_count, bool is_required) const {
    use();
    GLint location = glGetUniformLocation(id, p_name.c_str());
    if (location == -1) {
        if (is_required) {
            std::cout << "Uniform " << p_name << " not found or not used in shader program " << id << '.' << std::endl;
        }
        return;
    }
    GL_CALL(glUniform1fv, location, p_values_count, p_values);
}

void Shader::set_uniform(const std::string& p_name, const Framebuffer* p_fb, bool p_is_required) const {
    REQUIRE_NON_NULL(p_fb);
    use();
    auto texture_index = indices.find(p_fb);
    if (texture_index == indices.end()) {
        indices[p_fb] = indices.size();
        texture_index = indices.find(p_fb);
    }
    GLint location = GL_CALL(glGetUniformLocation, id, p_name.c_str());
    if (location == -1) {
        if (p_is_required) {
            std::cout << "Uniform " << p_name << " not found or not used in shader program " << id << '.' << std::endl;
        }
        return;
    }
    GL_CALL(glActiveTexture, GL_TEXTURE0 + texture_index->second);
    p_fb->bind_texture();
    GL_CALL(glUniform1i, location, texture_index->second);
}

void Shader::set_uniform(const std::string& p_name, const TextureCube* p_texture, bool p_is_required) const {
    REQUIRE_NON_NULL(p_texture);
    use();
    auto texture_index = indices.find(p_texture);
    if (texture_index == indices.end()) {
        indices[p_texture] = indices.size();
        texture_index = indices.find(p_texture);
    }
    GLint location = GL_CALL(glGetUniformLocation, id, p_name.c_str());
    if (location == -1) {
        if (p_is_required) {
            std::cout << "Uniform " << p_name << " not found or not used in shader program " << id << '.' << std::endl;
        }
        return;
    }
    GL_CALL(glActiveTexture, GL_TEXTURE0 + texture_index->second);
    p_texture->bind();
    GL_CALL(glUniform1i, location, texture_index->second);
}

void Shader::set_uniform(const std::string& p_name, const Texture2D* p_texture , bool p_is_required) const {
    REQUIRE_NON_NULL(p_texture);
    use();
    auto texture_index = indices.find(p_texture);
    if (texture_index == indices.end()) {
        indices[p_texture] = indices.size();
        texture_index = indices.find(p_texture);
    }
    GLint location = GL_CALL(glGetUniformLocation, id, p_name.c_str());
    if (location == -1) {
        if (p_is_required) {
            std::cout << "Uniform " << p_name << " not found or not used in shader program " << id << '.' << std::endl;
        }
        return;
    }
    GL_CALL(glActiveTexture, GL_TEXTURE0 + texture_index->second);
    p_texture->bind();
    GL_CALL(glUniform1i, location, texture_index->second);
}

void Shader::set_uniform(const std::string& p_name, const Texture3D* p_texture, bool p_is_required) const {
    REQUIRE_NON_NULL(p_texture);
    use();
    auto texture_index = indices.find(p_texture);
    if (texture_index == indices.end()) {
        indices[p_texture] = indices.size();
        texture_index = indices.find(p_texture);
    }
    GLint location = GL_CALL(glGetUniformLocation, id, p_name.c_str());
    if (location == -1) {
        if (p_is_required) {
            std::cout << "Uniform " << p_name << " not found or not used in shader program " << id << '.' << std::endl;
        }
        return;
    }
    GL_CALL(glActiveTexture, GL_TEXTURE0 + texture_index->second);
    p_texture->bind();
    GL_CALL(glUniform1i, location, texture_index->second);
}

void Shader::set_uniform(const std::string& p_name, const PointLight* p_light, bool p_is_required) const {
    REQUIRE_NON_NULL(p_light);
    use();
    set_uniform(p_name + ".position", p_light->get_position(), p_is_required);
    set_uniform(p_name + ".color", p_light->get_color(), p_is_required);
    set_uniform(p_name + ".far_plane", p_light->get_shadow_range(), p_is_required);
    set_uniform(p_name + ".attenuation", p_light->get_attenuation(), p_is_required);
    GLint shadow_map_location = GL_CALL(glGetUniformLocation, id, (p_name + ".shadow_map").c_str());
    if (shadow_map_location == -1) {
        if (p_is_required) {
            std::cout << "Uniform " << p_name << ".shadow_map not found or not used in shader program " << id << '.' << std::endl;
        }
    } else {
        int texture_index;
        auto texture_index_iter = indices.find(p_light);
        if (texture_index_iter == indices.end()) {
            texture_index = indices.size();
            indices[p_light] = texture_index;
        }
        else {
            texture_index = texture_index_iter->second;
        }
        GL_CALL(glActiveTexture, GL_TEXTURE0 + texture_index);
        p_light->bind_shadow_map();
        GL_CALL(glUniform1i, shadow_map_location, texture_index);
    }
    unuse();
}

void Shader::set_uniform(const std::string& p_name, const SpotLight* p_light, bool p_is_required) const {
    REQUIRE_NON_NULL(p_light);
    use();
    set_uniform(p_name + ".position", p_light->get_position(), p_is_required);
    set_uniform(p_name + ".color", p_light->get_color(), p_is_required);
    set_uniform(p_name + ".far_plane", p_light->get_shadow_range(), p_is_required);
    set_uniform(p_name + ".attenuation", p_light->get_attenuation(), p_is_required);
    set_uniform(p_name + ".light_view", p_light->get_light_view(), p_is_required);
    set_uniform(p_name + ".light_space", p_light->get_light_space(), p_is_required);
    set_uniform(p_name + ".gi_type", p_light->get_gi_type(), p_is_required);
    set_uniform(p_name + ".shadow_map_bias", p_light->get_shadow_map_bias(), p_is_required);
    GLint shadow_map_location = GL_CALL(glGetUniformLocation, id, (p_name + ".shadow_map").c_str());
    if (shadow_map_location == -1) {
        if (p_is_required) {
            std::cout << "Uniform " << p_name << ".shadow_map not found or not used in shader program " << id << '.' << std::endl;
        }
    } else {
        int texture_index;
        auto texture_index_iter = indices.find(p_light);
        if (texture_index_iter == indices.end()) {
            texture_index = indices.size();
            indices[p_light] = texture_index;
        }
        else {
            texture_index = texture_index_iter->second;
        }
        GL_CALL(glActiveTexture, GL_TEXTURE0 + texture_index);
        p_light->bind_shadow_map();
        GL_CALL(glUniform1i, shadow_map_location, texture_index);
    }
    switch (p_light->get_gi_type()) {
        case SpotLight::GIType::NONE: {
            auto default_texture = GlobalResource::get_singleton()->get_resource<Texture2DRGB>("default_texture_2d");
            set_uniform(p_name + ".normal_map", default_texture.get(), p_is_required);
            set_uniform(p_name + ".coordinate_map", default_texture.get(), p_is_required);
            set_uniform(p_name + ".flux_map", default_texture.get(), p_is_required);
        }   break;
        case SpotLight::GIType::LPV:
            // TODO
        case SpotLight::GIType::RSM:
            set_uniform(p_name + ".normal_map", p_light->get_normal_map().get(), p_is_required);
            set_uniform(p_name + ".coordinate_map", p_light->get_coordinate_map().get(), p_is_required);
            set_uniform(p_name + ".flux_map", p_light->get_flux_map().get(), p_is_required);
            break;
        default:
            throw std::runtime_error("Unsupported GI type.");
    };
}

std::string Shader::get_active_uniform_names() const {
    use();
    GLint count;
    GL_CALL(glGetProgramiv, id, GL_ACTIVE_UNIFORMS, &count);
    std::string names;
    for (int i = 0; i < count; i++) {
        char name[256];
        GLsizei length;
        GLint size;
        GLenum type;
        GL_CALL(glGetActiveUniform, id, i, 256, &length, &size, &type, name);
        names += name;
        names += " size: " + std::to_string(size) + " type: " + std::to_string(type) + "\n";

    }
    return names;
}
