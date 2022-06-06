#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

layout(set = 0, binding = 0) uniform global_data {
    mat4 projection;
    mat4 view;
} global_ubo;

layout(push_constant) uniform constants {
    mat4 model;
} object_ubo;

layout(location = 0) out VS_OUT {
    vec3 norm;
} vs_out;

void main() {

    vs_out.norm = vec3(normal);
    gl_Position = global_ubo.projection * global_ubo.view * object_ubo.model * vec4(pos, 1.0);
}