#version 460 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 vps[6];

out GS_OUT {
    vec4 pos;
} geom_out;

void main() {
    for (int face = 0; face < 6; ++face) {
        gl_Layer = face;
        for (int i = 0; i < gl_in.length(); ++i) {
            geom_out.pos = gl_in[i].gl_Position;
            gl_Position = vps[face] * geom_out.pos;
            EmitVertex();
        }
        EndPrimitive();
    }
}
