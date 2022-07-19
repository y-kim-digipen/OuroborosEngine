#version 450

#include "common_vert.glsl"

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

layout(location = 0) out VS_OUT {
    vec3 norm;
    vec3 frag_position;
    vec3 cam_pos;
    vec2 uv;
    vec3 non_pure_normal;
} vs_out;

layout(set = 1, binding = 0) uniform Test
{
	vec3 var;
} test;

void main() 
{
    vs_out.norm =  normal;
    vs_out.frag_position = vec4(object_ubo.model * vec4(pos,1.0)).xyz; 
    vs_out.cam_pos = test.var;
    vs_out.uv = uv;
    vs_out.non_pure_normal = object_ubo.normal_matrix*normal;
    gl_Position = global_ubo.projection * global_ubo.view * object_ubo.model * vec4(pos, 1.0);

}