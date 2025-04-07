#ifndef __APPLICATION_HOUSE_HUTAO_H__
#define __APPLICATION_HOUSE_HUTAO_H__
#include "animation/animator_skeletal.h"
#include "application.h"
#include "camera.h"
#include "data/skeleton.h"
#include "data/skeleton_model.h"
#include "window.h"
#include "framebuffer/framebuffer.h"
#include "scene_node/sn_skeleton_model.h"
#include "scene_node/sn_model.h"
#include <glm/gtc/matrix_transform.hpp> 

class ApplicationHouseHutao : public Application, public KeyboardInputEventObserver, public WindowResizeEventObserver {
public:

    ApplicationHouseHutao() = default;

    ApplicationHouseHutao(const ApplicationHouseHutao&) = delete;
    ApplicationHouseHutao(ApplicationHouseHutao&&) = delete;
    ApplicationHouseHutao& operator=(const ApplicationHouseHutao&) = delete;
    ApplicationHouseHutao& operator=(ApplicationHouseHutao&&) = delete;

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

    void pre_draw(double p_delta_time);
    void draw(double p_delta_time);
    void draw_ui(double p_delta_time);
    void post_draw(double p_delta_time);
    void init_global_resources();
    void init_resources();

    Window* application_window = nullptr;

    class FBRegular* framebuffer = nullptr;
    class FBDepth* framebuffer_depth = nullptr;
    class FBRegular* framebuffer_normal = nullptr;
    Framebuffer* framebuffer_id = nullptr;
    SceneNode* root_node = nullptr;
    SceneNode* light = nullptr;
    class SNModel* object = nullptr;
    class SNBox* floor = nullptr;
    class SNTestBox* test_box = nullptr;
    std::shared_ptr<SkeletonModel> mesh = nullptr;
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
};
#endif
