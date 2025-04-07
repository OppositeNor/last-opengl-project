#ifndef __MATERIAL_PBR_H__
#define __MATERIAL_PBR_H__

#include "material/material.hpp"
class MaterialPBR : public Material {
public:
    MaterialPBR(
        std::vector<std::shared_ptr<Texture2D>> p_albedo_map,
        std::vector<std::shared_ptr<Texture2D>> p_normal_map,
        std::vector<std::shared_ptr<Texture2D>> p_metalic_map,
        std::vector<std::shared_ptr<Texture2D>> p_roughness_map,
        std::vector<std::shared_ptr<Texture2D>> p_ao_map);

    virtual void set_uniform(bool p_required = false) const override;
    virtual void set_uniform(const char* p_param_name, bool p_required = false) const override;

    virtual void set_flux_uniform(bool p_required = false) const override;
    virtual void set_flux_uniform(const char* p_param_name, bool p_required = false) const override;

    virtual void set_depth_uniform(bool p_required = false) const override;
    virtual void set_depth_uniform(const char* p_param_name, bool p_required = false) const override;

private:
    std::vector<std::shared_ptr<Texture2D>> albedo_map;
    std::vector<std::shared_ptr<Texture2D>> normal_map;
    std::vector<std::shared_ptr<Texture2D>> metalic_map;
    std::vector<std::shared_ptr<Texture2D>> roughness_map;
    std::vector<std::shared_ptr<Texture2D>> ao_map;
};

#endif
