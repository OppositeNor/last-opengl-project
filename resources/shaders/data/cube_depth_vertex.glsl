#version 460 core

layout(location = VERTEX_LOCATION_POS) in vec3 vert_pos;

uniform mat4 model;

void main() {
    gl_Position = model * vec4(vert_pos, 1.0);
}
