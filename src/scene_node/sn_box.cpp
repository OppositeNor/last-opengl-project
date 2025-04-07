#include "scene_node/sn_box.h"
#include "point_light.h"
#include "spot_light.h"
#include "utils.hpp"
#include "camera.h"
#include "window.h"

SNBox::SNBox(Application* p_application, std::shared_ptr<const OBJModel> p_model)
    : SceneNodeBase(p_application), model(p_model), material(p_model->get_material()) {
}
SNBox::~SNBox() {
}

template<typename T>
void optional_set_if_not_empty(const Shader* shader, const std::optional<T>& value,
                               const std::string& name) {
    if (value.has_value()) {
        shader->set_uniform(name, value.value());
    }
}

void SNBox::draw(const struct RenderData& p_render_data) const {
    REQUIRE_NON_NULL(p_render_data.depth_buffer);

    auto shader = REQUIRE_NON_NULL(material)->get_shader();
    shader->reset();
    shader->use();
    shader->set_uniform("model", global_transform);
    shader->set_uniform("proj", p_render_data.proj_mat);
    shader->set_uniform("ambient_light", p_render_data.ambient_light);
    shader->set_uniform("far_plane", p_render_data.far_plane);
    material->set_uniform("material");
    p_render_data.camera->set_uniform(shader.get());

    int light_count = p_render_data.point_lights->size();
    shader->set_uniform("point_light_count", static_cast<int>(light_count));
    shader->set_uniform("lighting_enabled", lighting_enabled);
    int index = 0;
    for (auto* light : *(p_render_data.point_lights)) {
        shader->set_uniform("point_lights[" + std::to_string(index) + "]", REQUIRE_NON_NULL(light)->get_light().get());
        ++index;
    }
    for (size_t i = light_count; i < PointLight::MAX_COUNT; i++) {
        shader->set_uniform("point_lights[" + std::to_string(i) + "].shadow_map",
                            Window::get_default_texture_cube().get());
    }
    light_count = p_render_data.spot_lights->size();
    shader->set_uniform("spot_light_count", light_count);
    index = 0;
    for (auto* light : *(p_render_data.spot_lights)) {
        shader->set_uniform("spot_lights[" + std::to_string(index) + "]", REQUIRE_NON_NULL(light)->get_light().get());
        ++index;
    }
    for (size_t i = light_count; i < SpotLight::MAX_COUNT; ++i) {
        shader->set_uniform("spot_lights[" + std::to_string(i) + "].shadow_map",
                            Window::get_default_texture_2d().get());
        shader->set_uniform("spot_lights[" + std::to_string(i) + "].normal_map",
                            Window::get_default_texture_2d().get());
        shader->set_uniform("spot_lights[" + std::to_string(i) + "].coordinate_map",
                            Window::get_default_texture_2d().get());
        shader->set_uniform("spot_lights[" + std::to_string(i) + "].flux_map",
                            Window::get_default_texture_2d().get());
    }
    shader->set_uniform("depth_map", p_render_data.depth_buffer);
    shader->set_uniform("direct_lighting_enabled", p_render_data.direct_lighting_enabled);
    shader->set_uniform("gi_enabled", p_render_data.gi_enabled);
    model->draw();
    shader->unuse();
}

void SNBox::draw_transform_only(std::shared_ptr<const Shader> p_shader) const {
    p_shader->set_uniform("model", global_transform);
    model->draw();
}

void SNBox::draw_flux(std::shared_ptr<const SpotLight> p_spot_light) const {
    auto shader = REQUIRE_NON_NULL(material)->get_flux_shader();
    if (shader == nullptr) {
        return;
    }
    shader->use();
    shader->set_uniform("model", global_transform);
    shader->set_uniform("view", p_spot_light->get_light_view());
    shader->set_uniform("proj", p_spot_light->get_shadow_map_proj());
    shader->set_uniform("spot_light", p_spot_light.get());
    material->set_flux_uniform("material");
    model->draw();
    shader->unuse();
}

void SNBox::draw_depth(const RenderData& p_render_data) const {
    auto shader = REQUIRE_NON_NULL(material)->get_depth_shader();
    if (shader == nullptr) {
        return;
    }
    shader->use();
    shader->set_uniform("model", global_transform);
    shader->set_uniform("view", p_render_data.camera->get_view_matrix());
    shader->set_uniform("proj", p_render_data.proj_mat);
    shader->set_uniform("far_plane", p_render_data.far_plane);
    material->set_depth_uniform("material");
    model->draw();
    shader->unuse();
}
