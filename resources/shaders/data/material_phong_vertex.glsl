#version 460 core

layout(location = VERTEX_LOCATION_POS) in vec3 vert_pos;
layout(location = VERTEX_LOCATION_NORMAL) in vec3 vert_normal;
layout(location = VERTEX_LOCATION_UV) in vec2 vert_uv;
layout(location = VERTEX_LOCATION_TANGENT) in vec3 vert_tangent;
layout(location = VERTEX_LOCATION_BITANGENT) in vec3 vert_bitangent;

struct CameraV {
    mat4 view;
};

uniform CameraV camera_v;

uniform mat4 model;
uniform mat4 proj;

out VS_OUT {
    vec2 uv;
    vec3 pos;
    mat3 TBN;
} vert_out;

void main() {
    vert_out.pos = (model * vec4(vert_pos, 1.0)).xyz;
    gl_Position = proj * camera_v.view * vec4(vert_out.pos, 1.0);
    mat3 ti_model = transpose(inverse(mat3(model)));
    vec3 T = normalize(ti_model * vert_tangent);
    vec3 N = normalize(ti_model * vert_normal);
    vec3 B = normalize(ti_model * vert_bitangent);
    vert_out.TBN = mat3(T, B, N);
    vert_out.uv = vert_uv;
}
