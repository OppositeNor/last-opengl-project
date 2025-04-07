#version 460 core

layout(location = 0) in vec3 pos;

uniform mat4 view;
uniform mat4 proj;

out VS_OUT {
    vec3 pos;
}vert_out;

void main() {
    vert_out.pos = pos;
    gl_Position = proj * vec4((view * vec4(vert_out.pos, 0.0)).xyz, 1.0);
}
