#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv;

layout(location = 0) VS_OUT
{
    vec4 vertexPosition; // interpolated vPosition
    vec2 vertexUV; // interpolated vUV
} vs_out;

void main()
{
    vs_out.vertexPosition =  vec4( vPosition, 1.0f );
    vs_out.vertexUV = vUV;
    gl_Position = vs_out.vertexPosition;
}
