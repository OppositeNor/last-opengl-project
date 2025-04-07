#version 460 core

uniform samplerCube skybox;

in VS_OUT {
    vec3 pos;
}vert_out;

uniform sampler2D depth_map;

out vec4 color;

void main() {
    float depth = texture(depth_map, gl_FragCoord.xy / textureSize(depth_map, 0)).r;
    if (depth < 1.0) {
        discard;
    }
    vec3 dir = normalize(vert_out.pos);
    color = vec4(texture(skybox, dir).rgb, 1.0);
}
