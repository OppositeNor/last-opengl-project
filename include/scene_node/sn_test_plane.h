#ifndef __SN_TEST_PLANE_H__
#define __SN_TEST_PLANE_H__
#include "material/material_test_plane.h"
#include "scene_node/sn_box.h"
class SNTestPlane : public SNBox {
public:
    SNTestPlane(Application* p_application, std::shared_ptr<const OBJModel> p_model, int p_light_index)
        : SNBox(p_application, p_model) {
        test_material = std::make_shared<MaterialTestPlane>(p_light_index);
        material = test_material;
        disable_lighting();
        disable_shadow();
    };

    void set_display_map_index(int p_display_map_index) {
        test_material->set_display_map_index(p_display_map_index);
    }

    int get_display_map_index() {
        return test_material->get_display_map_index();
    }
private:
    std::shared_ptr<MaterialTestPlane> test_material;
};
#endif
