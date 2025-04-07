#ifndef __MATERIAL_PHONG_H__
#define __MATERIAL_PHONG_H__
#include "material/material.hpp"

class MaterialPhong : public Material{
public:

    MaterialPhong(std::vector<std::shared_ptr<Texture2D>> p_diffuse,
                  std::vector<std::shared_ptr<Texture2D>> p_normal,
                  std::vector<std::shared_ptr<Texture2D>> p_specular);

    virtual ~MaterialPhong() override {}

    virtual void set_uniform(bool p_required = false) const override;
    virtual void set_uniform(const char* p_param_name, bool p_required = false) const override;

    virtual void set_flux_uniform(bool p_required = false) const override;
    virtual void set_flux_uniform(const char* p_param_name, bool p_required = false) const override;

    virtual void set_depth_uniform(bool p_required = false) const override;
    virtual void set_depth_uniform(const char* p_param_name, bool p_required = false) const override;

private:
    std::vector<std::shared_ptr<Texture2D>> diffuse;
    std::vector<std::shared_ptr<Texture2D>> normal;
    std::vector<std::shared_ptr<Texture2D>> specular;
};
#endif
