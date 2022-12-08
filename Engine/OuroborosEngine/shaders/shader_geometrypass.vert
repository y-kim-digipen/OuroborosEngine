#version 450

#include "common_vert.glsl"

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 tangent;

layout(location = 0) out VS_OUT {
    vec3 norm;
    vec3 frag_position;
    vec3 cam_pos;
    vec2 uv;
    mat3 TBN;
} vs_out;

void main() 
{
    vs_out.norm = normalize(mat3(object_ubo.normal_matrix) * normal);
    vs_out.frag_position = vec3(global_ubo.view * object_ubo.model * vec4(pos,1.0)); 
    vs_out.cam_pos = global_ubo.cam_pos;
    vs_out.uv = uv;

    vec3 out_tangent = normalize(mat3(object_ubo.normal_matrix) * tangent);

    // more accurate tangent with a cheap cost
    out_tangent = normalize(out_tangent - dot(out_tangent,vs_out.norm) * vs_out.norm);
    vec3 bitangent = cross(vs_out.norm, out_tangent);
    vs_out.TBN = mat3(out_tangent, bitangent, vs_out.norm);

    gl_Position = global_ubo.projection * global_ubo.view * object_ubo.model * vec4(pos, 1.0);
}