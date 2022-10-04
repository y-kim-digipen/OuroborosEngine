#version 450

#include "common_frag.glsl"

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform Test {
    float att;
} oout;

layout (location=0) out vec3 vPos;
layout (location=1) out vec3 normal;
layout (location=2) out vec3 albedo;
layout (location=3) out vec3 emissive;
layout (location=4) out vec3 metalRoughnessAo;


void main()
{   
    vPos = vs_in.frag_pos;
    normal = normal(vs_in.non_pure_normal);
    if(material.has_normal_texture != 0)
    {
        normal = getNormalFromMap();
    }

    albedo = AlbedoValue();
    emissive = EmissiveValue();
    metalRoughnessAo = vec3(MetalicValue(), RoughnessValue(),AoValue());


} 