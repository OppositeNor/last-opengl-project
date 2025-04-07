#version 460 core

layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

struct Vertex {
    vec3 vert_pos;
    vec3 vert_normal;
    vec2 vert_texture_uv;
    vec3 vert_tangent;
    vec3 vert_bitangent;
    ivecMAX_BONE_INFLUENCE vert_bone_ids;
    vecMAX_BONE_INFLUENCE vert_bone_weights;
};

layout(std430, binding = 0) buffer debug_data_buffer {
    int debug_output[];
};

layout(std430, binding = 1) buffer default_vertex_buffer {
    Vertex default_vertices[];
};

layout(std430, binding = 2) buffer vertex_array_buffer {
    Vertex vertices[];
};

uniform sampler2D bones_transforms;
uniform sampler2D bones_offsets;
uniform int vert_count;

mat4 get_mat_from_texture(sampler2D texture, int row) {
    return mat4(texelFetch(texture, ivec2(0, row), 0),
                texelFetch(texture, ivec2(1, row), 0),
                texelFetch(texture, ivec2(2, row), 0),
                texelFetch(texture, ivec2(3, row), 0));
}

void main() {
    uint global_invocation_index = gl_GlobalInvocationID.x;
    debug_output[global_invocation_index] = int(global_invocation_index);
    if (global_invocation_index >= vert_count) {
        return;
    }
    Vertex vertex = default_vertices[global_invocation_index];
    vec4 final_pos = vec4(0.0);
    vec3 final_normal = vec3(0.0);
    bool found = false;
    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
        if (vertex.vert_bone_ids[i] == -1) {
            continue;
        }
        found = true;
        mat4 final_transform = get_mat_from_texture(bones_transforms, vertex.vert_bone_ids[i])
            * get_mat_from_texture(bones_offsets, vertex.vert_bone_ids[i]) * vertex.vert_bone_weights[i];
        final_pos += final_transform * vec4(vertex.vert_pos, 1.0);
        final_normal += mat3(transpose(inverse(final_transform))) * vertex.vert_normal;
    }
    if (!found) {
        final_pos = vec4(vertex.vert_pos, 1.0);
        final_normal = vertex.vert_normal;
    }
    vertices[global_invocation_index].vert_pos = final_pos.xyz;
    vertices[global_invocation_index].vert_normal = final_normal.xyz;
}
