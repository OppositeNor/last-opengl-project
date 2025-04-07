#ifndef __SN_MODEL_H__
#define __SN_MODEL_H__
#include "drawable.hpp"
#include "scene_node/scene_node.h"
#include "data/obj_model.h"
#include "shader/shader.h"
// A model object
class SNModel : public SceneNodeBase<SNModel>, public Drawable {
public:
    SNModel(Application* p_application, std::shared_ptr<const OBJModel> p_model);
    SNModel(const SNModel&) = delete;
    virtual ~SNModel();

    // Get the model
    std::shared_ptr<const OBJModel> get_model() const {
        return model;
    }

    virtual void draw(const RenderData& p_render_data) const override;

    virtual void draw_transform_only(std::shared_ptr<const Shader> p_shader) const override;

    virtual void draw_flux(std::shared_ptr<const SpotLight> p_spot_light) const override;

    virtual void draw_depth(const RenderData& p_render_data) const override;
private:
    std::shared_ptr<const OBJModel> model;
};

#endif
