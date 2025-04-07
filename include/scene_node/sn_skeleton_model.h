#ifndef __SN_SKELETON_MODEL_H__
#define __SN_SKELETON_MODEL_H__
#include "animation/animated_object.hpp"
#include "data/skeleton.h"
#include "drawable.hpp"
#include "scene_node/scene_node.h"
#include "data/skeleton_model.h"

class SNSkeletonModel : public SceneNodeBase<SNSkeletonModel>, public AnimatedObject, public Drawable {
public:
    SNSkeletonModel(Application* p_application, std::shared_ptr<const SkeletonModel> p_model, std::shared_ptr<const Skeleton> p_skeleton);

    SNSkeletonModel(const SNSkeletonModel&) = delete;
    virtual ~SNSkeletonModel();

    // Get the model
    std::shared_ptr<const SkeletonModel> get_model() const {
        return model;
    }

    // Get the skeleton
    std::shared_ptr<const Skeleton> get_skeleton() const {
        return skeleton;
    }

    virtual void update_animation() override;

    virtual void draw(const RenderData& p_render_data) const override;

    virtual void draw_transform_only(std::shared_ptr<const Shader> p_shader) const override;

    virtual void draw_flux(std::shared_ptr<const SpotLight> p_spot_light) const override;

    virtual void draw_depth(const RenderData& p_render_data) const override;

private:
    std::shared_ptr<const SkeletonModel> model;
    std::shared_ptr<const Skeleton> skeleton;

    void set_mesh_uniform(int p_index, const RenderData& p_render_data) const;
    void set_mesh_uniform_flux(int p_index, std::shared_ptr<const SpotLight> p_spot_light) const;
    void set_mesh_uniform_depth(int p_index, const RenderData& p_render_data) const;
    void draw_mesh(int p_index, const RenderData& p_render_data) const;
};

#endif
