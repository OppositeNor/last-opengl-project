#version 460 core

in VS_OUT {
    vec3 color;
} vert_out;

out vec4 color;

void main() {
    color = vec4(vert_out.color, 1.0);
}
