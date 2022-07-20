#version 450

#include "common_frag.glsl"

layout(location = 0) out vec4 outColor;

void main() 
{
    albedo = AlbedoValue();
    outColor = vec4(albedo, 1.0);
}