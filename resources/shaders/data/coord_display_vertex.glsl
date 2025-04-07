#version 460 core

layout(location = 0) in vec3 pos;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

out VS_OUT {
    vec3 color;
} vert_out;

void main() {
    vec3 pos = (model * vec4(pos, 1.0)).xyz;
    gl_Position = proj * vec4((view * vec4(pos, 0.0)).xyz, 1.0);
    vert_out.color = pos;
}
