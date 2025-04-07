#ifndef __SN_LIGHT_H__
#define __SN_LIGHT_H__
#include "scene_node/scene_node.h"
#include "point_light.h"

class SNPointLight : public SceneNodeBase<SNPointLight>, public PointLightObject {
public:

    SNPointLight(Application* p_application, std::shared_ptr<PointLight> p_light);

    virtual ~SNPointLight() override;

    virtual std::shared_ptr<PointLight> get_light() override {
        return light;
    }

    virtual std::shared_ptr<const PointLight> get_light() const override {
        return light;
    }

    // updates the point light
    virtual void update_point_light() const override {
        light->get_position() = get_global_transform() * glm::vec4(0, 0, 0, 1);
    }

private:
    std::shared_ptr<PointLight> light;
};

#endif
