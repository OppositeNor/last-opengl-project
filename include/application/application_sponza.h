#ifndef __APPLICATION_SPONZA_H__
#define __APPLICATION_SPONZA_H__
#include "application/application.h"
#include "camera.h"
#include "keyboard_input_event_observer.h"
#include "scene_node/sn_skeleton_model.h"
#include "scene_node/sn_spot_light.h"
#include "scene_node/sn_test_plane.h"
#include "window.h"
#include "window_resize_event_observer.h"

class ApplicationSponza : public Application, public KeyboardInputEventObserver, public WindowResizeEventObserver {
public:

    ApplicationSponza() = default;

    ApplicationSponza(const ApplicationSponza&) = delete;
    ApplicationSponza(ApplicationSponza&&) = delete;
    ApplicationSponza& operator=(const ApplicationSponza&) = delete;
    ApplicationSponza& operator=(ApplicationSponza&&) = delete;

    // Initializes the application.
    virtual void init(const char* p_window_title, int p_window_width, int p_window_height) override;

    // Returns true if the window should be closed.
    virtual bool window_should_close() override;

    // Called every fram in the main loop.
    virtual void process(double p_delta_time) override;

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

    Window* application_window = nullptr;

    Framebuffer* framebuffer = nullptr;
    Framebuffer* framebuffer_depth = nullptr;
    Framebuffer* framebuffer_normal = nullptr;
    SNSpotLight* spot_light = nullptr;
    SNTestPlane* test_plane = nullptr;
    SNSkeletonModel* mesh_model = nullptr;

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

    void init_global_resources();
    void init_resources();
    void init_framebuffers(int p_window_width, int p_window_height);
    void init_scene();
};

#endif
