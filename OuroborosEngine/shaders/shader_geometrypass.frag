#version 450

#include "common_frag.glsl"

// layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform Test {
    float att;
} oout;

layout (location=0) out vec3 vPosBuffer;
layout (location=1) out vec3 normalBuffer;
layout (location=2) out vec3 albedoBuffer;
layout (location=3) out vec3 emissiveBuffer;
layout (location=4) out vec3 metalRoughnessAoBuffer;


void main()
{   
    vPosBuffer = vs_in.frag_pos;
    //normalBuffer = normalize(vs_in.non_pure_normal);
    normalBuffer = normalize(vs_in.norm);
    // if(material.has_normal_texture != 0)
    // {
    //     normalBuffer = getNormalFromMap();
    // }

    albedoBuffer = AlbedoValue();
    emissiveBuffer = EmissiveValue();
    metalRoughnessAoBuffer = vec3(MetalicValue(), RoughnessValue(),AoValue());


} 