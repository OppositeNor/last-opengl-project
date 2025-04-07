#include "scene_node/sn_skeleton_model.h"
#include "application/application.h"
#include "camera.h"
#include "utils.hpp"
#include "window.h"
#include "point_light.h"
#include "spot_light.h"

SNSkeletonModel::SNSkeletonModel(Application* p_application, std::shared_ptr<const SkeletonModel> p_model,
                                 std::shared_ptr<const Skeleton> p_skeleton)
        : SceneNodeBase(p_application), model(REQUIRE_NON_NULL(p_model)), skeleton(REQUIRE_NON_NULL(p_skeleton)) {
}

SNSkeletonModel::~SNSkeletonModel() {
}

void SNSkeletonModel::update_animation() {
    model->update(skeleton);
}

void SNSkeletonModel::draw(const RenderData& p_render_data) const {
    int mesh_count = model->get_mesh_count();
    for (int i = 0; i < mesh_count; ++i) {
        draw_mesh(i, p_render_data);
    }
}

void SNSkeletonModel::draw_transform_only(std::shared_ptr<const Shader> p_shader) const {
    p_shader->set_uniform("model", global_transform);
    std::size_t mesh_count = model->get_mesh_count();
    for (std::size_t i = 0; i < mesh_count; ++i) {
        model->draw_mesh(i);
    }
}

void SNSkeletonModel::set_mesh_uniform(int p_index, const RenderData& p_render_data) const {
    if (p_index != 0 && model->get_material(p_index) == model->get_material(p_index - 1)) {
        return;
    }

    auto shader = REQUIRE_NON_NULL(model->get_material(p_index))->get_shader();
    shader->reset();
    shader->use();
    shader->set_uniform("model", global_transform);
    shader->set_uniform("proj", p_render_data.proj_mat);
    shader->set_uniform("lighting_enabled", lighting_enabled);
    shader->set_uniform("ambient_light", p_render_data.ambient_light);
    shader->set_uniform("far_plane", p_render_data.far_plane);
    shader->set_uniform("depth_map", p_render_data.depth_buffer);
    shader->set_uniform("direct_lighting_enabled", p_render_data.direct_lighting_enabled);
    shader->set_uniform("gi_enabled", p_render_data.gi_enabled);
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
    }
}

void SNSkeletonModel::set_mesh_uniform_flux(int p_index, std::shared_ptr<const SpotLight> p_spot_light) const {
    if (p_index != 0 && model->get_material(p_index) == model->get_material(p_index - 1)) {
        return;
    }
    auto shader = REQUIRE_NON_NULL(model->get_material(p_index))->get_flux_shader();
    shader->reset();
    shader->use();
    shader->set_uniform("model", global_transform);
    shader->set_uniform("view", p_spot_light->get_light_view());
    shader->set_uniform("proj", p_spot_light->get_shadow_map_proj());
    shader->set_uniform("spot_light", p_spot_light.get());
}

void SNSkeletonModel::set_mesh_uniform_depth(int p_index, const RenderData& p_render_data) const {
    if (p_index != 0 && model->get_material(p_index) == model->get_material(p_index - 1)) {
        return;
    }
    auto shader = REQUIRE_NON_NULL(model->get_material(p_index))->get_depth_shader();
    shader->reset();
    shader->use();
    shader->set_uniform("model", global_transform);
    shader->set_uniform("view", p_render_data.camera->get_view_matrix());
    shader->set_uniform("proj", p_render_data.proj_mat);
    shader->set_uniform("far_plane", p_render_data.far_plane);
}

void SNSkeletonModel::draw_mesh(int p_index, const RenderData& p_render_data) const {
    set_mesh_uniform(p_index, p_render_data);
    model->get_material(p_index)->set_uniform("material");
    model->draw_mesh(p_index);
}

void SNSkeletonModel::draw_flux(std::shared_ptr<const SpotLight> p_spot_light) const {
    unsigned int mesh_count = model->get_mesh_count();
    for (unsigned int i = 0; i < mesh_count; ++i) {
        set_mesh_uniform_flux(i, p_spot_light);
        model->get_material(i)->set_flux_uniform("material");
        model->draw_mesh(i);
    }
}

void SNSkeletonModel::draw_depth(const RenderData& p_render_data) const {
    unsigned int mesh_count = model->get_mesh_count();
    for (unsigned int i = 0; i < mesh_count; ++i) {
        set_mesh_uniform_depth(i, p_render_data);
        model->get_material(i)->set_depth_uniform("material");
        model->draw_mesh(i);
    }
}
