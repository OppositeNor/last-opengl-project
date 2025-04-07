#include "scene_node/sn_spot_light.h"

SNSpotLight::SNSpotLight(Application* p_application, std::shared_ptr<SpotLight> p_light)
    : SceneNodeBase(p_application), light(p_light) {
}
