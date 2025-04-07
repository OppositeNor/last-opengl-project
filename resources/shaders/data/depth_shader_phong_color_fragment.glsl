#version 460 core

uniform float far_plane;

in VS_OUT {
    vec4 pos;
} vert_out;
out float depth;

void main()
{
    float depth = length(vert_out.pos.xyz) / far_plane;
    gl_FragDepth = depth;
}
