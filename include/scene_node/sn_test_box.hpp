#ifndef __SN_TEST_BOX_H__
#define __SN_TEST_BOX_H__
#include "material/material_test_box.h"
#include "scene_node/sn_box.h"

class SNTestBox : public SNBox {
public:
    SNTestBox(Application* p_application, std::shared_ptr<const OBJModel> p_model)
        : SNBox(p_application, p_model) {
        material = std::make_shared<MaterialTestBox>();
    };

};

#endif
