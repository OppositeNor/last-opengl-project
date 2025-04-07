#ifndef __MMD_MATERIAL_H__
#define __MMD_MATERIAL_H__
#include "material/material.hpp"
class MaterialMMD : public Material {
public:
    MaterialMMD(const glm::vec4& p_diffuse_color, std::shared_ptr<Texture2D> p_diffuse,
                const glm::vec3& p_specular_color, std::shared_ptr<Texture2D> p_toon);

    virtual ~MaterialMMD() override {
    }

    virtual void set_uniform(bool p_required = false) const override;
    virtual void set_uniform(const char* p_param_name, bool p_required = false) const override;
    
    virtual void set_flux_uniform(bool p_required = false) const override;
    virtual void set_flux_uniform(const char* p_param_name, bool p_required = false) const override;

    virtual void set_depth_uniform(bool p_required = false) const override;
    virtual void set_depth_uniform(const char* p_param_name, bool p_required = false) const override;

private:
    glm::vec4 diffuse_color;
    std::shared_ptr<Texture2D> diffuse;
    glm::vec3 specular_color;
    std::shared_ptr<Texture2D> toon;
};
#endif
