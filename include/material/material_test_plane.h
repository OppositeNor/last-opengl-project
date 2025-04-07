#ifndef __MATERIAL_TEST_PLANE_H__
#define __MATERIAL_TEST_PLANE_H__
#include "material/material.hpp"
class MaterialTestPlane : public Material {
public:
    MaterialTestPlane(int p_light_index);
    virtual ~MaterialTestPlane() override {}

    virtual void set_uniform(bool p_is_required = false) const override {
        shader->set_uniform("light_index", light_index, p_is_required);
    }

    virtual void set_uniform(const char* p_name, bool p_is_required = false) const override {
        std::string name(p_name);
        shader->set_uniform(name + ".light_index", light_index, p_is_required);
        shader->set_uniform(name + ".display_map_index", display_map_index, p_is_required);
    }

    virtual void set_flux_uniform(bool = false) const override {}
    virtual void set_flux_uniform(const char*, bool = false) const override {}

    virtual void set_depth_uniform(bool = false) const override {}
    virtual void set_depth_uniform(const char*, bool = false) const override {}

    void set_display_map_index(int p_display_index) {
        display_map_index = p_display_index;
    }

    int get_display_map_index() {
        return display_map_index;
    }
    
private:
    int light_index;
    int display_map_index = 0;
};
#endif
