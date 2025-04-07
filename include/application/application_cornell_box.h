#ifndef __APPLICATION_CORNELL_BOX_H__
#define __APPLICATION_CORNELL_BOX_H__
#include "application/application.h"
#include "data/skeleton_model.h"
#include "scene_node/sn_box.h"
#include "scene_node/sn_skeleton_model.h"
#include "scene_node/sn_spot_light.h"
#include "scene_node/sn_test_plane.h"
#include "camera.h"
#include "window.h"
class ApplicationCornellBox : public Application, public KeyboardInputEventObserver, public WindowResizeEventObserver {
public:
    ApplicationCornellBox() {}

    virtual ~ApplicationCornellBox() {}
    ApplicationCornellBox(const ApplicationCornellBox&) = delete;
    ApplicationCornellBox(ApplicationCornellBox&&) = delete;
    ApplicationCornellBox& operator=(const ApplicationCornellBox&) = delete;
    ApplicationCornellBox& operator=(ApplicationCornellBox&&) = delete;


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
    } render_mode = REGULAR;

    RenderData render_data;

    Window* application_window = nullptr;

    FBRegular* framebuffer = nullptr;
    FBDepth* framebuffer_depth = nullptr;
    FBRegular* framebuffer_normal = nullptr;
    SNSpotLight* light = nullptr;
    SNBox* floor = nullptr;
    SNBox* left_wall = nullptr;
    SNBox* right_wall = nullptr;
    SNBox* back_wall = nullptr;
    SNTestPlane* test_plane = nullptr;
    SNSkeletonModel* mesh_model = nullptr;
    Camera camera;

    std::shared_ptr<SkeletonModel> mesh = nullptr;

    void init_global_resources();
    void init_resources();
    void init_framebuffers(int p_window_width, int p_window_height);
    void init_scene();

    glm::vec3 light_move_dir = glm::vec3(0);
};
#endif
