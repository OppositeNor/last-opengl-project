#version 460 core

in VS_OUT {
    vec3 pos;
} vert_out;

out vec4 frag_color;

void main() {
    frag_color = vec4(vert_out.pos, 1.0);
}
