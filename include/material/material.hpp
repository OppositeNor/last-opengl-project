#ifndef __MATERIAL_HPP__
#define __MATERIAL_HPP__
#include "shader/shader.h"
class Material {
public:

    constexpr static unsigned int MAX_TEXTURE_COUNT = 2;

    Material() {}
    virtual ~Material() {};

    virtual void set_uniform(bool p_required = false) const = 0;
    virtual void set_uniform(const char* p_param_name, bool p_required = false) const = 0;

    virtual void set_flux_uniform(bool p_required = false) const = 0;
    virtual void set_flux_uniform(const char* p_param_name, bool p_required = false) const = 0;

    virtual void set_depth_uniform(bool p_required = false) const = 0;
    virtual void set_depth_uniform(const char* p_param_name, bool p_required = false) const = 0;

    std::shared_ptr<Shader> get_shader() {
        return shader;
    }

    std::shared_ptr<const Shader> get_shader() const {
        return shader;
    }


    std::shared_ptr<Shader> get_flux_shader() {
        return flux_shader;
    }

    std::shared_ptr<const Shader> get_flux_shader() const {
        return flux_shader;
    }

    std::shared_ptr<Shader> get_depth_shader() {
        return depth_shader;
    }

    std::shared_ptr<const Shader> get_depth_shader() const {
        return depth_shader;
    }

protected:
    std::shared_ptr<Shader> shader = nullptr;
    std::shared_ptr<Shader> flux_shader = nullptr;
    std::shared_ptr<Shader> depth_shader = nullptr;
};
#endif
