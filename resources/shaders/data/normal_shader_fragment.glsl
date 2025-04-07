#version 460 core

in VS_OUT {
    vec3 normal;
} vert_out;

out vec4 frag_color;

void main() {
    frag_color = vec4((normalize(vert_out.normal) + 1.0) / 2.0, 1.0);
}
