#version 450

#include "common_vert.glsl"

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

void main() 
{
    gl_Position = global_ubo.projection * global_ubo.view * object_ubo.model * vec4(pos, 1.0);
}