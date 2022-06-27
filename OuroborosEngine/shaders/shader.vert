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
     vec4 trash2;
     vec2 trash3;
} test;

layout(set = 1, binding = 1) uniform Test2 {
     vec3 trash;
     vec4 trash2;
     vec4 trash3;
} test2;

layout(push_constant) uniform constants {
    mat4 model;
    mat3 normal_matrix;
} object_ubo;

layout(location = 0) out VS_OUT {
    vec3 norm;
    vec3 frag_position;
    vec3 cam_pos;
} vs_out;

void main() 
{
    vs_out.norm = object_ubo.normal_matrix * normal;
    vs_out.frag_position = vec4(object_ubo.model * vec4(pos,1.0)).xyz; 
    vs_out.cam_pos = test.trash;
    gl_Position = global_ubo.projection * global_ubo.view * object_ubo.model * vec4(pos, 1.0);
}