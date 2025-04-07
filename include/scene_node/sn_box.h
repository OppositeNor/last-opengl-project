#ifndef __SN_BOX_H__
#define __SN_BOX_H__
#include "drawable.hpp"
#include "scene_node/scene_node.h"
#include "data/obj_model.h"
#include "shader/shader.h"
// A model object
class SNBox : public SceneNodeBase<SNBox>, public Drawable {
public:
    SNBox(Application* p_application, std::shared_ptr<const OBJModel> model);
    SNBox(const SNBox&) = delete;
    virtual ~SNBox();

    // Get the model
    std::shared_ptr<const OBJModel> get_model() const {
        return model;
    }

    virtual void draw(const struct RenderData& p_render_data) const override;

    virtual void draw_transform_only(std::shared_ptr<const Shader> p_shader) const override;

    virtual void draw_flux(std::shared_ptr<const SpotLight> p_spot_light) const override;

    virtual void draw_depth(const RenderData& p_render_data) const override;
protected:
    std::shared_ptr<const OBJModel> model;
    std::shared_ptr<const Material> material;
};

#endif
