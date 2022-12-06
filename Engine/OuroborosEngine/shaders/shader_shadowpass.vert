#version 450

layout (location = 0) in vec3 inPos;
layout (location = 0) out int outInstanceIndex;

layout(push_constant) uniform constants {
    mat4 model;
    mat4 normal_matrix;
} object_ubo;

void main()
{
    outInstanceIndex = gl_InstanceIndex;
	gl_Position =  object_ubo.model * vec4(inPos,1.0f);
}