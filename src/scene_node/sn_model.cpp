#include "scene_node/sn_model.h"
#include "application/application.h"
#include "spot_light.h"
#include "camera.h"
#include "window.h"
#include "point_light.h"

SNModel::SNModel(Application* p_application, std::shared_ptr<const OBJModel> p_model)
    : SceneNodeBase(p_application), model(REQUIRE_NON_NULL(p_model)) {
}

SNModel::~SNModel() {
}

void texture_set_if_not_null(std::shared_ptr<const Shader> p_shader, const Texture2D* p_texture, const std::string& p_name) {
    if (p_texture != nullptr) {
        p_shader->set_uniform(p_name, p_texture);
    }
    else {
        p_shader->set_uniform(p_name, Window::get_default_texture_2d().get());
    }
}

void SNModel::draw(const RenderData& p_render_data) const {
    auto shader = REQUIRE_NON_NULL(model->get_material())->get_shader();
    shader->reset();
    shader->use();
    update_global_transform();
    shader->set_uniform("model", global_transform);
    shader->set_uniform("proj", p_render_data.proj_mat);
    shader->set_uniform("ambient_light", p_render_data.ambient_light);
    shader->set_uniform("far_plane", p_render_data.far_plane);
    model->get_material()->set_uniform("material");
    shader->set_uniform("lighting_enabled", lighting_enabled);
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
    auto default_texture_2d = Window::get_default_texture_2d().get();
    for (size_t i = light_count; i < SpotLight::MAX_COUNT; ++i) {
        shader->set_uniform("spot_lights[" + std::to_string(i) + "].shadow_map", default_texture_2d);
        shader->set_uniform("spot_lights[" + std::to_string(i) + "].normal_map", default_texture_2d);
        shader->set_uniform("spot_lights[" + std::to_string(i) + "].coordinate_map", default_texture_2d);
        shader->set_uniform("spot_lights[" + std::to_string(i) + "].flux_map", default_texture_2d);
    }
    shader->set_uniform("direct_lighting_enabled", p_render_data.direct_lighting_enabled);
    shader->set_uniform("gi_enabled", p_render_data.gi_enabled);

    shader->set_uniform("depth_map", p_render_data.depth_buffer);
    model->draw();
    shader->unuse();
}

void SNModel::draw_transform_only(std::shared_ptr<const Shader> p_shader) const {
    p_shader->set_uniform("model", global_transform);
    model->draw();
}

void SNModel::draw_flux(std::shared_ptr<const SpotLight> p_spot_light) const {
    auto shader = model->get_material()->get_flux_shader();
    shader->use();
    shader->set_uniform("model", global_transform);
    shader->set_uniform("view", p_spot_light->get_light_view());
    shader->set_uniform("proj", p_spot_light->get_shadow_map_proj());
    shader->set_uniform("spot_light", p_spot_light.get());
    model->get_material()->set_flux_uniform("material");
    shader->unuse();
}

void SNModel::draw_depth(const RenderData& p_render_data) const {
    auto shader = model->get_material()->get_depth_shader();
    shader->use();
    shader->set_uniform("model", global_transform);
    shader->set_uniform("view", p_render_data.camera->get_view_matrix());
    shader->set_uniform("proj", p_render_data.proj_mat);
    shader->set_uniform("far_plane", p_render_data.far_plane);
    model->get_material()->set_depth_uniform("material");
    shader->unuse();
}

