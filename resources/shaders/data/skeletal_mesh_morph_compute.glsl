#version 460 core

layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 texture_uv;
    vec3 tangent;
    vec3 bitangent;
    ivecMAX_BONE_INFLUENCE bone_ids;
    vecMAX_BONE_INFLUENCE bone_weights;
};

struct MorphKeyData {
    vec3 position;
    vec3 normal;
    vec2 texture_uv;
    vec3 tangent;
    vec3 bitangent;
};

layout(std430, binding = 0) buffer debug_data_buffer {
    int debug_output[];
};

layout(std430, binding = 1) buffer default_vertex_buffer {
    Vertex default_vertices[];
};

layout(std430, binding = 2) buffer morph_data_buffer {
    MorphKeyData morph_key_data[];
};

layout(std430, binding = 3) buffer vertex_array_buffer {
    Vertex vertices[];
};

uniform float morph_weights[MORPH_NUM_MAX];
uniform int morph_counts;
uniform int vert_count;

void main() {
    uint global_invocation_index = gl_GlobalInvocationID.x;
    if (global_invocation_index >= vert_count) {
        return;
    }
    uint vertex_index;
    vertices[global_invocation_index].position    = default_vertices[global_invocation_index].position;
    vertices[global_invocation_index].normal      = default_vertices[global_invocation_index].normal;
    vertices[global_invocation_index].texture_uv  = default_vertices[global_invocation_index].texture_uv;
    vertices[global_invocation_index].tangent     = default_vertices[global_invocation_index].tangent;
    vertices[global_invocation_index].bitangent   = default_vertices[global_invocation_index].bitangent;
    for (uint i = 0; i < morph_counts; ++i) {
        float weight = morph_weights[i];
        if (weight != 0.0) {
            vertex_index = global_invocation_index + vert_count * i;
            vertices[global_invocation_index].position   += morph_key_data[vertex_index].position * weight;
            vertices[global_invocation_index].normal     += morph_key_data[vertex_index].normal * weight;
            vertices[global_invocation_index].texture_uv += morph_key_data[vertex_index].texture_uv * weight;
            vertices[global_invocation_index].tangent    += morph_key_data[vertex_index].tangent * weight;
            vertices[global_invocation_index].bitangent  += morph_key_data[vertex_index].bitangent * weight;
        }
    }
}
