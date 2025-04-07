#ifndef __APPLICATION_SPARTAN_H__
#define __APPLICATION_SPARTAN_H__
#include "animation/animator_skeletal.h"
#include "application.h"
#include "camera.h"
#include "data/skeleton.h"
#include "window.h"
#include "framebuffer/framebuffer.h"
#include "scene_node/sn_skeleton_model.h"
#include "scene_node/sn_point_light.h"
#include "scene_node/sn_spot_light.h"
#include "scene_node/sn_test_box.hpp"
#include "scene_node/sn_test_plane.h"
#include <glm/gtc/matrix_transform.hpp> 

class ApplicationSpartan : public Application, public KeyboardInputEventObserver, public WindowResizeEventObserver {
public:

    ApplicationSpartan() = default;

    ApplicationSpartan(const ApplicationSpartan&) = delete;
    ApplicationSpartan(ApplicationSpartan&&) = delete;
    ApplicationSpartan& operator=(const ApplicationSpartan&) = delete;
    ApplicationSpartan& operator=(ApplicationSpartan&&) = delete;

    // Initializes the application.
    virtual void init(const char* window_title, int window_width, int window_height) override;

    // Returns true if the window should be closed.
    virtual bool window_should_close() override;

    // Called every fram in the main loop.
    virtual void process(double delta_time) override;

    // Terminates the application.
    virtual void terminate() override;

    // Called when a key is pressed
    virtual void on_key_pressed(const std::string& p_key) override;

    // Called when a key is released
    virtual void on_key_released(const std::string& p_key) override;

    virtual void on_resize(unsigned int p_updated_x, unsigned int p_updated_y) override;
private:
    enum RenderMode {
        REGULAR = 0,
        DEPTH,
        NORMAL,
        RENDER_MODE_COUNT
    } render_mode;

    void pre_draw(double p_delta_time);
    void draw(double p_delta_time);
    void draw_ui(double p_delta_time);
    void post_draw(double p_delta_time);
    void init_global_resources();
    void init_resources();

    Window* application_window = nullptr;

    Framebuffer* framebuffer = nullptr;
    Framebuffer* framebuffer_depth = nullptr;
    Framebuffer* framebuffer_normal = nullptr;
    SNPointLight* light = nullptr;
    SNSpotLight* spot_light = nullptr;
    SNTestBox* test_box = nullptr;
    SNTestPlane* test_plane = nullptr;
    SNSkeletonModel* mesh_model = nullptr;

    AnimatorSkeletal* mesh_model_animator = nullptr;
    std::vector<std::string> mesh_model_anim_names;
    std::vector<const char*> mesh_model_anim_names_c_str;
    Skeleton* mesh_model_skeleton = nullptr;

    RenderData render_data;

    // camera
    Camera camera;

    constexpr static float CAMERA_SPEED = 1.00f;
    constexpr static float CAMERA_ROTATION_SPEED = 1.00f;

    bool draw_skeleton = false;
    bool should_draw_ui = true;

    glm::vec3 light_move_dir;

    void init_resource_loader();

    bool tick_tock = false;

    void depth_draw_pass();
    void normal_draw_pass();
    void result_draw_pass();
    void draw_framebuffer();
};
#endif

