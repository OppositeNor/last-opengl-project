#version 460 core

uniform float far_plane;

in VS_OUT {
    vec4 pos;
    vec2 uv;
} vert_out;

out float depth;

struct Material {
    int albedo_map_count;
    sampler2D albedo_map[Material::MAX_TEXTURE_COUNT];
};

uniform Material material;

void main() {
    if (material.albedo_map_count > 0 && texture(material.albedo_map[0], vert_out.uv).a == 0) {
        discard;
    }
    float depth = length(vert_out.pos.xyz) / far_plane;
    gl_FragDepth = depth;
}
