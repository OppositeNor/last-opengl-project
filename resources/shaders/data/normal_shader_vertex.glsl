#version 460 core

layout(location = VERTEX_LOCATION_POS) in vec3 vert_pos;
layout(location = VERTEX_LOCATION_NORMAL) in vec3 vert_normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out VS_OUT {
    vec3 normal;
} vert_out;

void main() {
    gl_Position = proj * view * model * vec4(vert_pos, 1.0);
    vert_out.normal = mat3(transpose(inverse(model))) * vert_normal;
}
