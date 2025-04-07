#include "coordinate_display.h"
#include "resource/global_resource.h"
#include "graphics.h"
#include "shader/shader_pipeline.h"

CoordinateDisplay::CoordinateDisplay() {
    shader = REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<ShaderPipeline>("shader_coord_display"));
}

CoordinateDisplay::~CoordinateDisplay() {
    if (vao != 0) {
        GL_CALL(glDeleteBuffers, 1, &vbo);
        GL_CALL(glDeleteVertexArrays, 1, &vao);
    }
}

void CoordinateDisplay::render(const glm::mat4& p_model, const RenderData* p_render_data) const {
    if (vao == 0) {
        init_draw();
    }
    // TODO
}

void CoordinateDisplay::init_draw() const {
    VertexData vertices_data[6];
    vertices_data[0].pos = glm::vec3(0.0, 0.0, 0.0);
    vertices_data[1].pos = glm::vec3(1.0, 0.0, 0.0);
    vertices_data[2].pos = glm::vec3(0.0, 0.0, 0.0);
    vertices_data[3].pos = glm::vec3(0.0, 1.0, 0.0);
    vertices_data[4].pos = glm::vec3(0.0, 0.0, 0.0);
    vertices_data[5].pos = glm::vec3(0.0, 0.0, 1.0);
    GL_CALL(glGenVertexArrays, 1, &vao);
    GL_CALL(glGenBuffers, 1, &vbo);
    GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vbo);
    GL_CALL(glBufferData, GL_ARRAY_BUFFER, sizeof(vertices_data), vertices_data, GL_STATIC_DRAW);
    // TODO
}
