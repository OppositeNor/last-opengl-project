#ifndef __SHADER_H__
#define __SHADER_H__
#include "utils.hpp"
#include <string>
#include <glm/glm.hpp>
#include <unordered_map>

class Shader {
public:
    Shader() {}

    Shader(const Shader&) = delete;

    virtual ~Shader();

    // reset the shader
    void reset() const;

    // Use the shader
    void use() const;

    void unuse() const;

    // Set a uniform for a 4x4 matrix
    void set_uniform(const std::string& name, const glm::mat4& value, bool is_required = false) const;

    // Set a uniform for a 3D vector
    void set_uniform(const std::string& name, const glm::vec3& value, bool is_required = false) const;

    // Set a uniform for a 3D float tuple
    void set_uniform(const std::string& name, const Tuple<float, 3>& value, bool is_required = false) const;

    // Set a uniform for an integer
    void set_uniform(const std::string& name, int value, bool is_required = false) const;

    // Set a uniform for a float
    void set_uniform(const std::string& name, float value, bool is_required = false) const;

    // Set a uniform for a float array
    void set_uniform(const std::string& p_name, float p_values[], int p_values_count, bool is_required = false) const;

    // Set a uniform for a texture of a framebuffer
    void set_uniform(const std::string& name, const class Framebuffer* fb, bool is_required = false) const;

    // Set a uniform for a texture cube
    void set_uniform(const std::string& p_name, const class TextureCube* p_texture, bool p_is_required = false) const;

    // Set a uniform for a 2D texture
    void set_uniform(const std::string& p_name, const class Texture2D* p_texture, bool p_is_required = false) const;

    // Set a uniform for a 3D texture
    void set_uniform(const std::string& p_name, const class Texture3D* p_texture, bool p_is_required = false) const;

    // Set a uniform for a point light
    void set_uniform(const std::string& p_name, const class PointLight* p_light, bool p_is_required = false) const;

    // Set a uniform for an area light
    void set_uniform(const std::string& p_name, const class SpotLight* p_light, bool p_is_required = false) const;

    // Get the names of the active uniforms
    std::string get_active_uniform_names() const;

protected:
    unsigned int id = 0;
    mutable std::unordered_map<const void*, int> indices;
};

#endif
