#ifndef __SN_COLOR_BOX_HPP__
#define __SN_COLOR_BOX_HPP__

#include "material/material_phong_color.h"
#include "scene_node/sn_box.h"

class SNColorBox : public SNBox {
public:
    SNColorBox(Application* p_application, std::shared_ptr<OBJModel> p_model,
               const glm::vec3& p_diffuse_color, const glm::vec3& p_specular_color)
        : SNBox(p_application, p_model) {
        pc_material = std::make_shared<MaterialPhongColor>(p_diffuse_color, p_specular_color);
        pc_material->set_diffuse_color(p_diffuse_color);
        pc_material->set_specular_color(p_specular_color);
        material = pc_material;
    }

    void set_diffuse_color(const glm::vec3& p_diffuse_color) {
        pc_material->set_diffuse_color(p_diffuse_color);
    }

    void set_specular_color(const glm::vec3& p_specular_color) {
        pc_material->set_specular_color(p_specular_color);
    }

    const glm::vec3& get_diffuse_color() const {
        return pc_material->get_diffuse_color();
    }

    glm::vec3& get_diffuse_color() {
        return pc_material->get_diffuse_color();
    }

    const glm::vec3& get_specular_color() const {
        return pc_material->get_specular_color();
    }

    glm::vec3& get_specular_color() {
        return pc_material->get_specular_color();
    }


private:
    std::shared_ptr<MaterialPhongColor> pc_material;
};
#endif
