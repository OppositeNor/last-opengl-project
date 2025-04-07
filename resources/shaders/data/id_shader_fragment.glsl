#version 460 core

out vec4 color;

uniform int object_id;

void main() {
    gl_FragDepth = float(object_id);
}
