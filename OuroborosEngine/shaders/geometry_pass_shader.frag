#version 450

layout (location = 0) out vec4 outPosition;
layout (location = 1) out vec4 outNormal;
layout (location = 2) out vec4 outAlbedo;

layout(location = 0) in VS_IN 
{
    vec3 norm;
    vec3 frag_position;
    vec3 cam_pos;
} vs_in;

layout(location = 1) in vec3 in_uv;


layout(set = 2, binding = 0) uniform Material 
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
} material;


void main()
{
    outPosition = vec4(vs_in.frag_position,1.0);
    outNormal = vec4(normalize(vs_in.norm),1.0);
    outAlbedo = vec4(material.diffuse,1.0);
}