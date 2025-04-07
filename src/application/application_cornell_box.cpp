#include "application/application_cornell_box.h"
#include "camera.h"
#include "data/obj_model_deserializer.h"
#include "data/skeletal_animation_data.h"
#include "data/skeletal_animation_deserializer.h"
#include "framebuffer/fb_regular.h"
#include "resource/global_resource.h"
#include "scene_node/sn_color_box.hpp"
#include "scene_node/sn_spot_light.h"
#include "scene_node/sn_test_plane.h"
#include "shader/shader_compute_deserializer.h"
#include "shader/shader_pipeline.h"
#include "shader/shader_pipeline_deserializer.h"
#include "system/system.h"
#include "texture/texture_2d_rgb_deserializer.h"
#include "texture/texture_cube_deserializer.h"
#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>

void ApplicationCornellBox::init(const char* p_window_title, int p_window_width, int p_window_height) {
    init_global_resources();
    WindowProp window_prop(p_window_width, p_window_height, false, p_window_title);
    application_window = new Window(window_prop);
    application_window->add_key_event_observer(this);
    application_window->add_window_resize_event_observer(this);
    application_window->add_key_event_observer(&camera);
    init_resources();
    init_framebuffers(p_window_width, p_window_height);
    init_scene();
    render_data.far_plane = 20.0f;
    render_data.proj_mat = glm::perspective(glm::radians(45.0f), (float)p_window_width/(float)p_window_height, 0.1f, render_data.far_plane);
    render_data.camera = &camera;
    render_data.gi_enabled = true; // (⌐■_■)
    render_data.ambient_light = glm::vec3(0, 0, 0);
}

void ApplicationCornellBox::init_framebuffers(int p_window_width, int p_window_height) {
    TextureConfigurations regular_config;
    regular_config.srgb = false;
    regular_config.scale_nearest_min = false;
    regular_config.scale_nearest_mag = false;
    TextureConfigurations depth_config;
    depth_config.srgb = false;
    depth_config.scale_nearest_min = true;
    depth_config.scale_nearest_mag = true;
    framebuffer = new FBRegular(p_window_width, p_window_height, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.05f, 0.05f, 0.08f, 1.0f), regular_config,
                                nullptr, REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<ShaderPipeline>("shader_fb_blur")));
    framebuffer_depth = new FBDepth(p_window_width, p_window_height, depth_config,
                                    nullptr, REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<ShaderPipeline>("shader_fb_red")));
    framebuffer_normal = new FBRegular(p_window_width, p_window_height, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.1f, 0.2f, 0.5f, 1.0f), regular_config,
                                       REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<ShaderPipeline>("shader_normal")),
                                       REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<ShaderPipeline>("shader_fb_regular")));
}

void ApplicationCornellBox::init_scene() {
    floor = new SNColorBox(this, REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<OBJModel>("plane")),
                           glm::vec3(1, 1, 1), glm::vec3(0, 0, 0));
    floor->get_local_transform().rotate(-glm::pi<float>() * 0.5, 1, 0, 0);
    floor->get_local_transform().translate(0, 0, 0);
    back_wall = new SNColorBox(this, REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<OBJModel>("plane")),
                               glm::vec3(0, 1, 0), glm::vec3(0, 0, 0));
    back_wall->get_local_transform().translate(0, 1, -1);
    left_wall = new SNColorBox(this, REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<OBJModel>("plane")),
                               glm::vec3(1, 1, 0), glm::vec3(0, 0, 0));
    left_wall->get_local_transform().rotate(glm::pi<float>() * 0.5, 0, 1, 0);
    left_wall->get_local_transform().translate(-1, 1, 0);
    right_wall = new SNColorBox(this, REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<OBJModel>("plane")),
                               glm::vec3(0, 0, 1), glm::vec3(0, 0, 0));
    right_wall->get_local_transform().rotate(-glm::pi<float>() * 0.5, 0, 1, 0);
    right_wall->get_local_transform().translate(1, 1, 0);
    auto mesh_model_data = GlobalResource::get_singleton()->get_resource<SkeletalAnimationData>("miku");
    mesh_model = new SNSkeletonModel(this, mesh_model_data->get_skeleton_model(), mesh_model_data->get_skeleton());
    mesh_model->get_local_transform().scale(0.05, 0.05, 0.05);
    light = new SNSpotLight(this, SpotLight::SpotLightBuilder()
                            .set_prop(glm::vec3(1.0, 1.0, 1.0), 3.0, 50.0, 2048)
                            .set_spot_light(glm::pi<float>() * 0.5, 0.1)
                            .set_rsm_gi(256).build_shared());
    light->get_local_transform().rotate(-glm::pi<float>() * 0.5, 1, 0, 0);
    light->get_local_transform().translate(0, 3, 0);
    test_plane = new SNTestPlane(this, REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<OBJModel>("plane")), 0);
    light->add_child(test_plane);
}

bool ApplicationCornellBox::window_should_close() {
    return application_window != nullptr && application_window->should_close();
}

void ApplicationCornellBox::process(double p_delta_time) {
    camera.update(p_delta_time);
    light->get_local_transform().translate(light_move_dir * (float)p_delta_time);

    application_window->before_render();
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
            framebuffer_normal->disable_depth_test();
            framebuffer_normal->draw(&render_data);
        case RenderMode::RENDER_MODE_COUNT:
            break;
    }
    framebuffer->unbind();
    application_window->after_render();
}

void ApplicationCornellBox::terminate() {
    delete application_window;
    delete framebuffer;
    delete framebuffer_depth;
    delete framebuffer_normal;
    delete light;
    delete floor;
    if (test_plane != nullptr) {
        delete test_plane;
    }
    delete mesh_model;
}

void ApplicationCornellBox::init_global_resources() {
    std::string exe_directory = System::get_singleton()->get_exe_directory();
    GlobalResource::get_singleton()->add_deserializer(std::make_shared<ShaderComputeDeserializer>());
    GlobalResource::get_singleton()->add_deserializer(std::make_shared<OBJModelDeserializer>());
    GlobalResource::get_singleton()->add_deserializer(std::make_shared<Texture2DRGBDeserializer>());
    GlobalResource::get_singleton()->add_deserializer(std::make_shared<ShaderPipelineDeserializer>());
    GlobalResource::get_singleton()->add_deserializer(std::make_shared<SkeletalAnimationDataDeserializer>());
    GlobalResource::get_singleton()->add_deserializer(std::make_shared<TextureCubeDeserializer>());
    GlobalResource::get_singleton()->load_global_resources(exe_directory + "/" + GlobalResource::GLOBAL_RES_HEADERS_PATH);
}

void ApplicationCornellBox::init_resources() {
    std::cout << "Loading resources..." << std::endl;
    auto exe_directory = System::get_singleton()->get_exe_directory();
    std::cout << "Loading shaders..." << std::endl;
    GlobalResource::get_singleton()->load_resources(exe_directory + "/shaders/shader_resources.toml");
    std::cout << "Loading models..." << std::endl;
    GlobalResource::get_singleton()->load_resources(exe_directory + "/models/model_resources.toml");
    std::cout << "Load resources done." << std::endl;
}

void ApplicationCornellBox::on_key_pressed(const std::string& p_key) {
    if (p_key == "change_render_mode") {
        render_mode = static_cast<RenderMode>((render_mode + 1) % RenderMode::RENDER_MODE_COUNT);
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
    if (p_key == "switch_test_plane_display_map") {
        test_plane->set_display_map_index((test_plane->get_display_map_index() + 1) % 4);
    }
    if (p_key == "toggle_direct_light") {
        render_data.direct_lighting_enabled = !render_data.direct_lighting_enabled;
    }
    if (p_key == "toggle_gi") {
        render_data.gi_enabled = !render_data.gi_enabled;
    }
}

void ApplicationCornellBox::on_key_released(const std::string& p_key) {
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

void ApplicationCornellBox::on_resize(unsigned int p_updated_x, unsigned int p_updated_y) {
    framebuffer->update_size(p_updated_x, p_updated_y);
    framebuffer_depth->update_size(p_updated_x, p_updated_y);
    framebuffer_normal->update_size(p_updated_x, p_updated_y);
}

