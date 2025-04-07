#ifndef __MATERIAL_PHONG_COLOR_H__
#define __MATERIAL_PHONG_COLOR_H__
#include "material/material.hpp"
class MaterialPhongColor : public Material {
public:

    MaterialPhongColor(const glm::vec3& p_diffuse_color,
                       const glm::vec3& p_specular_color);

    virtual ~MaterialPhongColor() override {}

    virtual void set_uniform(bool p_required = false) const override;
    virtual void set_uniform(const char* p_param_name, bool p_required = false) const override;

    virtual void set_flux_uniform(bool p_required = false) const override;
    virtual void set_flux_uniform(const char* p_param_name, bool p_required = false) const override;

    virtual void set_depth_uniform(bool = false) const override {}
    virtual void set_depth_uniform(const char*, bool = false) const override {}

    void set_diffuse_color(const glm::vec3& p_diffuse_color) {
        diffuse_color = p_diffuse_color;
    }

    void set_specular_color(const glm::vec3& p_specular_color) {
        specular_color = p_specular_color;
    }

    const glm::vec3& get_diffuse_color() const {
        return diffuse_color;
    }

    glm::vec3& get_diffuse_color() {
        return diffuse_color;
    }

    const glm::vec3& get_specular_color() const {
        return specular_color;
    }

    glm::vec3& get_specular_color() {
        return specular_color;
    }

private:
    glm::vec3 diffuse_color;
    glm::vec3 specular_color;
};
#endif
