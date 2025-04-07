#include "camera.h"
#include "utils.hpp"
#include "graphics.h"
#include <cmath>
#include <glm/ext/matrix_transform.hpp>

Camera::Camera(float p_camera_speed) : position(0.0f, 0.0f, 1.0f), direction(0.0f, 0.0f, -1.0f),
    velocity(0, 0, 0), rotation(0, 0), camera_speed(p_camera_speed) {

}

Camera::~Camera() {
    if (vao != 0) {
        GL_CALL(glDeleteVertexArrays, 1, &vao);
        GL_CALL(glDeleteBuffers, 1, &vbo);
        vao = 0;
        vbo = 0;
    }
}

void Camera::update(double p_delta_time) {
    move_forward(velocity.z * camera_speed * static_cast<float>(p_delta_time));
    move_right(velocity.x * camera_speed * static_cast<float>(p_delta_time));
    move_up(velocity.y * camera_speed * static_cast<float>(p_delta_time));
    rotate_pitch(rotation.x * CAMERA_ROTATION_SPEED * static_cast<float>(p_delta_time));
    rotate_yaw(rotation.y * CAMERA_ROTATION_SPEED * static_cast<float>(p_delta_time));
}

void Camera::set_uniform(const Shader* p_shader) const {
    p_shader->set_uniform("camera_v.view", get_view_matrix());
    p_shader->set_uniform("camera_f.pos", position);
}


void Camera::move_forward(float p_distance) {
    position += direction * p_distance;
}

void Camera::move_right(float p_distance) {
    position += glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f))) * p_distance;
}

void Camera::move_up(float p_distance) {
    position += glm::vec3(0.0f, 1.0f, 0.0f) * p_distance;
}

void Camera::rotate_pitch(float p_angle) {
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), p_angle, glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)));
    direction = glm::vec3(rotation * glm::vec4(direction, 0.0f));
}

void Camera::rotate_yaw(float p_angle) {
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), p_angle, glm::vec3(0.0f, 1.0f, 0.0f));
    direction = glm::vec3(rotation * glm::vec4(direction, 0.0f));
}

glm::mat4 Camera::get_view_matrix() const {
    return glm::lookAt(position, position + direction, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::on_key_pressed(const std::string& p_key) {
    if (p_key == "camera_forward") {
        velocity.z += 1.0f;
    }
    if (p_key == "camera_backward") {
        velocity.z += -1.0f;
    }
    if (p_key == "camera_left") {
        velocity.x += -1.0f;
    }
    if (p_key == "camera_right") {
        velocity.x += 1.0f;
    }
    if (p_key == "camera_up") {
        velocity.y += 1.0f;
    }
    if (p_key == "camera_down") {
        velocity.y += -1.0f;
    }
    if (p_key == "camera_rotate_pitch_up") {
        rotation.x += 1.0f;
    }
    if (p_key == "camera_rotate_pitch_down") {
        rotation.x += -1.0f;
    }
    if (p_key == "camera_rotate_yaw_left") {
        rotation.y += 1.0f;
    }
    if (p_key == "camera_rotate_yaw_right") {
        rotation.y += -1.0f;
    }
    velocity.x = std::max(-1.0f, std::min(1.0f, velocity.x));
    velocity.y = std::max(-1.0f, std::min(1.0f, velocity.y));
    velocity.z = std::max(-1.0f, std::min(1.0f, velocity.z));
    rotation.x = std::max(-1.0f, std::min(1.0f, rotation.x));
    rotation.y = std::max(-1.0f, std::min(1.0f, rotation.y));
}

void Camera::on_key_released(const std::string& p_key) {
    if (p_key == "camera_forward") {
        velocity.z -= 1.0f;
    }
    if (p_key == "camera_backward") {
        velocity.z -= -1.0f;
    }
    if (p_key == "camera_left") {
        velocity.x -= -1.0f;
    }
    if (p_key == "camera_right") {
        velocity.x -= 1.0f;
    }
    if (p_key == "camera_up") {
        velocity.y -= 1.0f;
    }
    if (p_key == "camera_down") {
        velocity.y -= -1.0f;
    }
    if (p_key == "camera_rotate_pitch_up") {
        rotation.x -= 1.0f;
    }
    if (p_key == "camera_rotate_pitch_down") {
        rotation.x -= -1.0f;
    }
    if (p_key == "camera_rotate_yaw_left") {
        rotation.y -= 1.0f;
    }
    if (p_key == "camera_rotate_yaw_right") {
        rotation.y -= -1.0f;
    }
    velocity.x = std::max(-1.0f, std::min(1.0f, velocity.x));
    velocity.y = std::max(-1.0f, std::min(1.0f, velocity.y));
    velocity.z = std::max(-1.0f, std::min(1.0f, velocity.z));
    rotation.x = std::max(-1.0f, std::min(1.0f, rotation.x));
    rotation.y = std::max(-1.0f, std::min(1.0f, rotation.y));
}

void Camera::draw_skybox(const RenderData& p_render_data) const {
    if (skybox_shader == nullptr || skybox == nullptr) {
        return;
    }
    if (vao == 0) {
        init_draw_data();
    }
    skybox_shader->use();
    skybox_shader->set_uniform("view", get_view_matrix());
    skybox_shader->set_uniform("proj", p_render_data.proj_mat);
    skybox_shader->set_uniform("skybox", skybox.get());
    skybox_shader->set_uniform("depth_map", p_render_data.depth_buffer);
    GL_CALL(glBindVertexArray, vao);
    GL_CALL(glDrawArrays, GL_TRIANGLES, 0, 36);
    GL_CALL(glBindVertexArray, 0);
    skybox_shader->unuse();
}

void Camera::init_draw_data() const {
    GL_CALL(glGenVertexArrays, 1, &vao);
    GL_CALL(glBindVertexArray, vao);
    GL_CALL(glGenBuffers, 1, &vbo);
    GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vbo);
    GL_CALL(glBufferData, GL_ARRAY_BUFFER, sizeof(inverse_box_vertices), inverse_box_vertices, GL_STATIC_DRAW);
    GL_CALL(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    GL_CALL(glEnableVertexAttribArray, 0);
    GL_CALL(glBindVertexArray, 0);
}

