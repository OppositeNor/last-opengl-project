#include "application/application_house_hutao.h"
#include "scene_node/scene_node.h"
#include "data/skeletal_animation_deserializer.h"
#include "framebuffer/fb_depth.h"
#include "framebuffer/fb_regular.h"
#include "data/obj_model_deserializer.h"
#include "resource/global_resource.h"
#include "scene_node/sn_point_light.h"
#include "scene_node/sn_test_box.hpp"
#include "shader/shader_compute_deserializer.h"
#include "shader/shader_pipeline_deserializer.h"
#include "system/system.h"
#include "texture/texture_2d_rgb_deserializer.h"
#include "texture/texture_cube_deserializer.h"
#include "utils.hpp"
#include <cstdio>
#include <glm/ext/scalar_constants.hpp>
#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>

void ApplicationHouseHutao::init(const char* p_window_title, int p_window_width, int p_window_height) {
    init_global_resources();
    WindowProp window_prop(p_window_width, p_window_height, false, p_window_title);
    application_window = new Window(window_prop);
    application_window->add_key_event_observer(this);
    application_window->add_window_resize_event_observer(this);
    init_resources();

    // object = new SNModel(this, GlobalResource::get_singleton()->get_resource<OBJModel>("house"));

    auto hutao_sa_data = REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<SkeletalAnimationData>("hutao"));
    mesh_model_skeleton = hutao_sa_data->get_skeleton().get();

    mesh = hutao_sa_data->get_skeleton_model();
    mesh_model = new SNSkeletonModel(this, mesh, hutao_sa_data->get_skeleton());
    // mesh_model->get_local_transform().scale(glm::vec3(0.1, 0.1, 0.1));
    // mesh_model->get_local_transform().rotate(glm::pi<float>() / -2.0, 1.0, 0.0, 0.0);

    mesh_model_animator = hutao_sa_data->get_animator_skeletal().get();

    floor = new SNBox(this, GlobalResource::get_singleton()->get_resource<OBJModel>("cube"));
    floor->get_local_transform().scale(glm::vec3(10.0f, 1.0f, 10.0f));
    floor->get_local_transform().translate(0.0f, -1.0f, 0.0f);

    light = new SNPointLight(this, std::make_shared<PointLight>(glm::vec3(0.3f, 0.34f, 0.4f), 20.0f));
    light->get_local_transform().translate(3.0f, 3.0f, 3.0f);

    root_node = new SceneNode(this);
    root_node->add_child(light);
    root_node->add_child(floor);
    root_node->add_child(mesh_model);

    mesh_model->get_local_transform().translate(0.0f, 0.0f, 0.0f);
    mesh_model->disable_lighting();

    render_data.far_plane = 20.0f;
    render_data.proj_mat = glm::perspective(glm::radians(45.0f), (float)p_window_width/(float)p_window_height, 0.1f, render_data.far_plane);
    render_data.camera = &camera;
    application_window->add_key_event_observer(&camera);

    TextureConfigurations regular_config;
    regular_config.srgb = false;
    regular_config.scale_nearest_min = false;
    regular_config.scale_nearest_mag = false;
    TextureConfigurations depth_config;
    depth_config.srgb = false;
    depth_config.scale_nearest_min = true;
    depth_config.scale_nearest_mag = true;
    framebuffer = new FBRegular(p_window_width, p_window_height, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.1f, 0.2f, 0.5f, 1.0f), regular_config,
                                nullptr, REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<ShaderPipeline>("shader_fb_regular")));
    framebuffer_depth = new FBDepth(p_window_width, p_window_height, depth_config,
                                    nullptr, REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<ShaderPipeline>("shader_fb_red")));
    framebuffer_normal = new FBRegular(p_window_width, p_window_height, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.1f, 0.2f, 0.5f, 1.0f), regular_config,
                                       REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<ShaderPipeline>("shader_normal")),
                                       REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<ShaderPipeline>("shader_fb_regular")));
    framebuffer_id = new FBDepth(p_window_width, p_window_height, depth_config,
                                 REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<ShaderPipeline>("shader_id")), nullptr);
    test_box = new SNTestBox(this, GlobalResource::get_singleton()->get_resource<OBJModel>("cube"));
    test_box->disable_shadow();
    light->add_child(test_box);
    test_box->get_local_transform().scale(0.25, 0.25, 0.25);


    mesh_model_anim_names = mesh_model_animator->get_animations_names();
    mesh_model_anim_names_c_str.reserve(mesh_model_anim_names.size());
    for (auto& name : mesh_model_anim_names) {
        mesh_model_anim_names_c_str.push_back(name.c_str());
    }
}

void ApplicationHouseHutao::init_global_resources() {
    std::string exe_directory = System::get_singleton()->get_exe_directory();
    GlobalResource::get_singleton()->add_deserializer(std::make_shared<ShaderComputeDeserializer>());
    GlobalResource::get_singleton()->add_deserializer(std::make_shared<OBJModelDeserializer>());
    GlobalResource::get_singleton()->add_deserializer(std::make_shared<Texture2DRGBDeserializer>());
    GlobalResource::get_singleton()->add_deserializer(std::make_shared<ShaderPipelineDeserializer>());
    GlobalResource::get_singleton()->add_deserializer(std::make_shared<SkeletalAnimationDataDeserializer>());
    GlobalResource::get_singleton()->add_deserializer(std::make_shared<TextureCubeDeserializer>());
    GlobalResource::get_singleton()->load_global_resources(exe_directory + "/" + GlobalResource::GLOBAL_RES_HEADERS_PATH);
}

void ApplicationHouseHutao::init_resources() {
    std::cout << "Loading resources..." << std::endl;
    auto exe_directory = System::get_singleton()->get_exe_directory();
    std::cout << "Loading shaders..." << std::endl;
    GlobalResource::get_singleton()->load_resources(exe_directory + "/shaders/shader_resources.toml");
    std::cout << "Loading models..." << std::endl;
    GlobalResource::get_singleton()->load_resources(exe_directory + "/models/model_resources.toml");
    std::cout << "Load resources done." << std::endl;
}

void ApplicationHouseHutao::process(double p_delta_time) {
    // object->get_local_transform().rotate(0.5f * p_delta_time, glm::vec3(0.0f, 1.0f, 0.0f));
    mesh_model_animator->update(p_delta_time);
    mesh->reset_meshes();
    mesh_model_animator->apply_bones(mesh_model_skeleton);
    mesh_model_animator->apply_morphs(mesh.get());
    light->get_local_transform().translate(light_move_dir * (float)p_delta_time);
    mesh_model->update_animation();

    pre_draw(p_delta_time);
    draw(p_delta_time);
    post_draw(p_delta_time);
}

void ApplicationHouseHutao::terminate() {
    // delete object;
    delete mesh_model;
    delete floor;
    delete light;
    delete root_node;
    delete framebuffer;
    delete framebuffer_depth;
    delete framebuffer_normal;
    GlobalResource::get_singleton()->clear_resources();
    if (test_box != nullptr) {
        delete test_box;
    }
    delete application_window;
}

void ApplicationHouseHutao::on_key_pressed(const std::string& p_key) {
    if (p_key == "change_render_mode") {
        render_mode = static_cast<RenderMode>((render_mode + 1) % RenderMode::RENDER_MODE_COUNT);
    }
    if (p_key == "toggle_draw_skeleton") {
        draw_skeleton = !draw_skeleton;
    }
    if (p_key == "toggle_ui") {
        should_draw_ui = !should_draw_ui;
    }
    auto camera_dir = glm::normalize(glm::vec3(camera.get_direction().x, 0.0, camera.get_direction().z));
    if (p_key == "light_forward") {
        light_move_dir += camera_dir;
    }
    if (p_key == "light_backward") {
        light_move_dir += -camera_dir;
    }
    if (p_key == "light_left") {
        light_move_dir += normalize(glm::cross(glm::vec3(0.0, 1.0, 0.0), camera_dir));
    }
    if (p_key == "light_right") {
        light_move_dir += -normalize(glm::cross(glm::vec3(0.0, 1.0, 0.0), camera_dir));
    }
    if (p_key == "light_up") {
        light_move_dir += glm::vec3(0.0, 1.0, 0.0);
    }
    if (p_key == "light_down") {
        light_move_dir += glm::vec3(0.0, -1.0, 0.0);
    }
}

void ApplicationHouseHutao::on_key_released(const std::string& p_key) {
    auto camera_dir = glm::normalize(glm::vec3(camera.get_direction().x, 0.0, camera.get_direction().z));
    if (p_key == "light_forward") {
        light_move_dir -= camera_dir;
    }
    if (p_key == "light_backward") {
        light_move_dir -= -camera_dir;
    }
    if (p_key == "light_left") {
        light_move_dir -= normalize(glm::cross(glm::vec3(0.0, 1.0, 0.0), camera_dir));
    }
    if (p_key == "light_right") {
        light_move_dir -= -normalize(glm::cross(glm::vec3(0.0, 1.0, 0.0), camera_dir));
    }
    if (p_key == "light_up") {
        light_move_dir -= glm::vec3(0.0, 1.0, 0.0);
    }
    if (p_key == "light_down") {
        light_move_dir -= glm::vec3(0.0, -1.0, 0.0);
    }
}

void ApplicationHouseHutao::on_resize(unsigned int p_updated_x, unsigned int p_updated_y) {
    framebuffer->update_size(p_updated_x, p_updated_y);
    framebuffer_depth->update_size(p_updated_x, p_updated_y);
    framebuffer_id->update_size(p_updated_x, p_updated_y);
    framebuffer_normal->update_size(p_updated_x, p_updated_y);
}


void ApplicationHouseHutao::pre_draw(double) {
    framebuffer->unbind();
    application_window->before_render();
    render_data.ambient_light = glm::vec3(1.0, 1.0, 1.0);
}

void ApplicationHouseHutao::draw(double p_delta_time) {
    render_data.point_lights = &point_light_objects;
    render_data.spot_lights = &spot_light_objects;

    // depth
    framebuffer_depth->bind();
    framebuffer_depth->enable_depth_test();
    for (auto* drawable : drawables) {
        drawable->draw_depth(render_data);
    }
    framebuffer_depth->unbind();
    render_data.depth_buffer = framebuffer_depth;

    // normal
    framebuffer_normal->bind();
    framebuffer_normal->enable_depth_test();
    framebuffer_normal->get_shader()->use();
    framebuffer_normal->get_shader()->set_uniform("view", render_data.camera->get_view_matrix());
    framebuffer_normal->get_shader()->set_uniform("proj", render_data.proj_mat);
    for (auto* drawable : drawables) {
        drawable->draw_transform_only(framebuffer_normal->get_shader());
    }
    framebuffer_normal->get_shader()->unuse();
    framebuffer_normal->unbind();
    render_data.normal_buffer = framebuffer_normal;

    // result
    framebuffer->bind();
    framebuffer->enable_depth_test();
    for (auto* drawable : drawables) {
        drawable->draw(render_data);
    }
    framebuffer->unbind();

    switch(render_mode) {
        case RenderMode::REGULAR:
            framebuffer->disable_depth_test();
            framebuffer->draw(&render_data);
            break;
        case RenderMode::DEPTH:
            framebuffer->disable_depth_test();
            framebuffer_depth->draw(&render_data);
            break;
        case RenderMode::NORMAL:
            framebuffer->disable_depth_test();
            framebuffer_normal->draw(&render_data);
            break;
        case RenderMode::RENDER_MODE_COUNT:
            break;
    }
    if (draw_skeleton) {
        mesh_model_skeleton->render(mesh_model->get_global_transform(), render_data);
    }

    if (should_draw_ui) {
        draw_ui(p_delta_time);
    }
    camera.update(p_delta_time);
}

void ApplicationHouseHutao::draw_ui(double p_delta_time) {
    ImGui::Begin("Debug window");
    ImGui::Text("Framerate: %.02f", 1.0 / p_delta_time);
    ImGui::Text("Memory Usage: %s", System::get_singleton()->get_program_memory_usage().c_str());
    ImGui::Text("Current camera pos: %f, %f, %f", camera.get_position().x, camera.get_position().y, camera.get_position().z);
    ImGui::Checkbox("Draw skeleton", &draw_skeleton);
    if (ImGui::Button("Reset pose")) {
        mesh_model_animator->play_animation(-1);
        mesh_model_skeleton->reset_pose();
    }
    ImGui::Text("Animations");
    int curr_anim = mesh_model_animator->get_current_animation_index();
    if (ImGui::Combo("##Animations", &curr_anim, mesh_model_anim_names_c_str.data(), mesh_model_anim_names.size())) {
        mesh_model_animator->play_animation(curr_anim);
    }
    if (mesh_model_animator->get_current_animation_index() != -1) {
        auto& current_anim = mesh_model_animator->get_animations()[mesh_model_animator->get_current_animation_index()];
        float current_time = mesh_model_animator->get_current_time();
        ImGui::SliderFloat("Current time", &current_time, 0.0, current_anim.get_duration());
        mesh_model_animator->set_current_time(current_time);
        float anim_speed = mesh_model_animator->get_animation_playspeed();
        if (ImGui::SliderFloat("Animation speed", &anim_speed, 0.0, 1.0)) {
            mesh_model_animator->set_animation_playspeed(anim_speed);
        }
    }
    int highlighting_bone = mesh_model_skeleton->get_highlighting_bone();
    if (ImGui::InputInt("Highlight bone", &highlighting_bone)) {
        mesh_model_skeleton->highlight_bone(highlighting_bone);
    }
    auto& bones_infos = mesh_model_skeleton->get_bone_infos();
    if (highlighting_bone >= 0 && (std::size_t)highlighting_bone < bones_infos.size()) {
        ImGui::Text("Highlighting bone name: %s", bones_infos[highlighting_bone].bone_name.c_str());
    }
    ImGui::Text("Resources");
    if (ImGui::BeginListBox("##ResourcesList")) {
        auto resources = GlobalResource::get_singleton()->get_resource_ids();
        for (auto resource : resources) {
            ImGui::Text("%s", resource.c_str());
        }
        ImGui::EndListBox();
    }
    ImGui::End();
}

void ApplicationHouseHutao::post_draw(double) {
    application_window->after_render();
}

bool ApplicationHouseHutao::window_should_close() {
    return application_window->should_close();
}

