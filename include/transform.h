#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__
#include <glm/glm.hpp>

class Transform {
public:
    Transform();
    ~Transform() {}

    // Translate
    void translate(float p_x, float p_y, float p_z);
    void translate(const glm::vec3& p_vec);

    // Rotate
    void rotate(float p_angle, float p_x, float p_y, float p_z);
    void rotate(float p_angle, const glm::vec3& p_vec);

    // Scale
    void scale(float p_x, float p_y, float p_z);
    void scale(const glm::vec3& p_vec);

    // Reset
    void reset() {
        matrix = glm::mat4(1.0);
    }

    // Get matrix
    const glm::mat4& get_matrix() const {
        return matrix;
    }
    
    // Get matrix
    glm::mat4& get_matrix() {
        return matrix;
    }
private:
    glm::mat4 matrix;
};



#endif
