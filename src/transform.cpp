#include "transform.h"
#include <glm/gtc/matrix_transform.hpp>

Transform::Transform() {
    reset();
}

void Transform::translate(float p_x, float p_y, float p_z) {
    matrix = glm::translate(glm::mat4(1.0), glm::vec3(p_x, p_y, p_z)) * matrix;
}

void Transform::translate(const glm::vec3& p_vec) {
    matrix = glm::translate(glm::mat4(1.0), p_vec) * matrix;
}

void Transform::rotate(float p_angle, float p_x, float p_y, float p_z) {
    matrix = glm::rotate(glm::mat4(1.0), p_angle, glm::vec3(p_x, p_y, p_z)) * matrix;
}

void Transform::rotate(float p_angle, const glm::vec3& p_vec) {
    matrix = glm::rotate(glm::mat4(1.0), p_angle, p_vec) * matrix;
}

void Transform::scale(float p_x, float p_y, float p_z) {
    matrix = glm::scale(glm::mat4(1.0), glm::vec3(p_x, p_y, p_z)) * matrix;
}

void Transform::scale(const glm::vec3& p_vec) {
    matrix = glm::scale(glm::mat4(1.0), p_vec) * matrix;
}
