#version 460 core

layout(location = VERTEX_LOCATION_POS) in vec3 vert_pos;
layout(location = VERTEX_LOCATION_BONE_ID) in ivecMAX_BONE_INFLUENCE vert_bone_ids;
layout(location = VERTEX_LOCATION_BONE_WEIGHT) in vecMAX_BONE_INFLUENCE vert_bone_weights;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform bool enabled_bones;

uniform sampler2D bones_offsets;
uniform sampler2D bones_transforms;

mat4 get_mat_from_texture(sampler2D texture, int row) {
    return mat4(
            texelFetch(texture, ivec2(0, row), 0),
            texelFetch(texture, ivec2(1, row), 0),
            texelFetch(texture, ivec2(2, row), 0),
            texelFetch(texture, ivec2(3, row), 0));
}

void main() {
    if (enabled_bones) {
        vec4 final_pos = vec4(0.0);
        bool found = false;
        for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
            if (vert_bone_ids[i] == -1) {
                continue;
            }
            found = true;
            mat4 final_transform = get_mat_from_texture(bones_transforms, vert_bone_ids[i])
                * get_mat_from_texture(bones_offsets, vert_bone_ids[i]) * vert_bone_weights[i];
            final_pos += final_transform * vec4(vert_pos, 1.0);
        }
        if (!found) {
            final_pos = vec4(vert_pos, 1.0);
        }
        gl_Position = proj * view * model * vec4(final_pos.xyz, 1.0);
    }
    else {
        gl_Position = proj * view * model * vec4(vert_pos, 1.0);
    }
}
