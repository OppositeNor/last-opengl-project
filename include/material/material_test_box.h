#ifndef __MATERIAL_TEST_BOX_H__
#define __MATERIAL_TEST_BOX_H__
#include "material/material.hpp"
class MaterialTestBox : public Material {
public:
    MaterialTestBox();
    virtual void set_uniform(bool = false) const override {}
    virtual void set_uniform(const char*, bool = false) const override {}

    virtual void set_flux_uniform(bool = false) const override {}
    virtual void set_flux_uniform(const char*, bool = false) const override {}

    virtual void set_depth_uniform(bool = false) const override {}
    virtual void set_depth_uniform(const char*, bool = false) const override {}

};
#endif
