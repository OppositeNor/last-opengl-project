#ifndef __CAMERA_H__
#define __CAMERA_H__
#include "keyboard_input_event_observer.h"
#include "shader/shader_pipeline.h"
#include "texture/texture_cube.h"
#include <glm/glm.hpp>

class Camera : public KeyboardInputEventObserver {
public:

    static constexpr float CAMERA_ROTATION_SPEED = 1.0f;

    Camera(float p_camera_speed = 1.0f);
    virtual ~Camera() override;

    void set_skybox(std::shared_ptr<TextureCube> p_skybox) {
        skybox = p_skybox;
    }

    std::shared_ptr<const TextureCube> get_skybox() const {
        return skybox;
    }

    std::shared_ptr<TextureCube> get_skybox() {
        return skybox;
    }

    // set camera uniform
    void set_uniform(const Shader* p_shader) const;

    // called every frame
    void update(double p_delta_time);

    // move forward
    void move_forward(float p_distance);

    // move right
    void move_right(float p_distance);

    // move up
    void move_up(float p_distance);

    // rotate pitch
    void rotate_pitch(float p_angle);

    // rotate yaw
    void rotate_yaw(float p_angle);

    // get view matrix
    glm::mat4 get_view_matrix() const;

    // get position
    const glm::vec3& get_position() const {
        return position;
    }

    glm::vec3& get_position() {
        return position;
    }

    const glm::vec3& get_direction() const {
        return direction;
    }

    float get_speed() const {
        return camera_speed;
    }

    void set_speed(float p_speed) {
        camera_speed = p_speed;
    }

    void set_skybox_shader(std::shared_ptr<ShaderPipeline> p_skybox_shader) {
        skybox_shader = p_skybox_shader;
    }

    std::shared_ptr<const ShaderPipeline> get_skybox_shader() const {
        return skybox_shader;
    }

    std::shared_ptr<ShaderPipeline> get_skybox_shader() {
        return skybox_shader;
    }

    void draw_skybox(const RenderData& p_render_data) const;

    // called when a key is pressed.
    virtual void on_key_pressed(const std::string& p_key) override;

    // called when a key is released.
    virtual void on_key_released(const std::string& p_key) override;
private:
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 velocity;
    glm::vec2 rotation;
    float camera_speed;
    std::shared_ptr<TextureCube> skybox = nullptr;
    std::shared_ptr<ShaderPipeline> skybox_shader = nullptr;
    mutable unsigned int vbo = 0, vao = 0;
    void init_draw_data() const;
};
#endif
