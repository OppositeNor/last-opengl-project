#version 460 core

in GS_OUT {
    vec4 pos;
} geom_out;

uniform vec3 light_pos;
uniform float far_plane;

void main() {
    gl_FragDepth = length(geom_out.pos.xyz - light_pos) / far_plane;
}
