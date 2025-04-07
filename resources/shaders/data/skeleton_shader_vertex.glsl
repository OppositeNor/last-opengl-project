#version 460 core

layout(location = 0) in vec3 color;
layout(location = 1) in int  bone_id;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform int highlighted_bone_id;

uniform sampler2D bones_transforms;

out VS_OUT {
    vec3 color;
} vert_out;

mat4 get_mat_from_texture(sampler2D texture, int row) {
    return mat4(texelFetch(texture, ivec2(0, row), 0),
                texelFetch(texture, ivec2(1, row), 0),
                texelFetch(texture, ivec2(2, row), 0),
                texelFetch(texture, ivec2(3, row), 0));
}

void main() {
    mat4 bone_transform = get_mat_from_texture(bones_transforms, bone_id);
    gl_Position = proj * view * model * bone_transform * vec4(0.0, 0.0, 0.0, 1.0);
    if (bone_id == highlighted_bone_id) {
        vert_out.color = vec3(1.0, 0.0, 0.0);
    }
    else {
        vert_out.color = color;
    }
}
