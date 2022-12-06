#version 450

#include "common_vert.glsl"

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 4) in vec3 tangent;
layout(location = 5) in vec3 bitangent;

layout(location = 0) out VS_OUT {
    vec3 norm;
    vec3 frag_position;
    vec3 cam_pos;
    vec2 uv;
    vec3 non_pure_normal;
    mat3 TBN;
    mat3 normal_matrix;
} vs_out;

void main() 
{
    vs_out.norm = normalize(mat3(object_ubo.normal_matrix) * normal);
    vs_out.frag_position = vec3(object_ubo.model * vec4(pos,1.0)); 
    vs_out.cam_pos = global_ubo.cam_pos;
    vs_out.uv = uv;
    vs_out.non_pure_normal = mat3(object_ubo.model) * normal;
    vs_out.normal_matrix = mat3(object_ubo.normal_matrix);
    
    vec4 t_ = object_ubo.model* vec4(tangent, 0.0);
    vec4 b_ = object_ubo.model* vec4(bitangent, 0.0);
    vec4 n_ = object_ubo.model* vec4(normal, 0.0);

    vec3 T = normalize(vec3(t_));
    vec3 B = normalize(vec3(b_));
    vec3 N = normalize(vec3(n_));
    vs_out.TBN = mat3(T,B,N);
    
    gl_Position = global_ubo.projection * global_ubo.view * object_ubo.model * vec4(pos, 1.0);

}