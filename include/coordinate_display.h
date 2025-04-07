#ifndef __COORDINATE_DISPLAY_H__
#define __COORDINATE_DISPLAY_H__

#include "shader/shader.h"
#include "utils.hpp"
class CoordinateDisplay {
public:
    CoordinateDisplay();
    virtual ~CoordinateDisplay();

    void render(const glm::mat4& p_model, const RenderData* p_render_data) const;
private:
    mutable unsigned int vao = 0, vbo = 0;

    struct VertexData {
        glm::vec3 pos;
    };

    void init_draw() const;

    std::shared_ptr<Shader> shader;
};

#endif
