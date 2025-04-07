#include "scene_node/sn_point_light.h"

SNPointLight::SNPointLight(Application* p_application, std::shared_ptr<PointLight> p_light)
    : SceneNodeBase(p_application), light(p_light) {
}

SNPointLight::~SNPointLight() {
}

