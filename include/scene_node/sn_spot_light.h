#ifndef __SN_SPOT_LIGHT_H__
#define __SN_SPOT_LIGHT_H__

#include "spot_light.h"
#include "scene_node/scene_node.h"
class SNSpotLight : public SceneNodeBase<SNSpotLight>, public SpotLightObject {
public:
    SNSpotLight(Application* p_application, std::shared_ptr<SpotLight> p_light);

    virtual ~SNSpotLight() override {}

    // Get the light
    virtual std::shared_ptr<SpotLight> get_light() override { return light; }

    // Get the light
    virtual std::shared_ptr<const SpotLight> get_light() const override { return light; }

    // updates the area light
    virtual void update_spot_light() const override {
        light->update_light_space(get_global_transform());
    }

private:
    std::shared_ptr<SpotLight> light;
};

#endif
