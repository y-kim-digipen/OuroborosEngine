#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

layout(set = 0, binding = 0) uniform global_data {
    mat4 projection;
    mat4 view;
    vec3 cam_pos;
} global_ubo;

layout(set = 1, binding = 0) uniform Test {
     vec3 trash;
}test;

layout(push_constant) uniform constants {
    mat4 model;
    mat3 normal_matrix;
} object_ubo;

layout(location = 0) out VS_OUT {
    vec3 norm;
    vec3 frag_position;
} vs_out;

void main() 
{
    vs_out.norm = normal;
    vs_out.frag_position = vec4(object_ubo.model * vec4(pos,1.0)).xyz; 
    gl_Position = global_ubo.projection * global_ubo.view * object_ubo.model * vec4(pos, 1.0);
}