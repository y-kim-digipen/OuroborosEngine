#version 450

#define LIGHT_COUNT 3

layout (triangles, invocations = LIGHT_COUNT) in;
layout (triangle_strip, max_vertices = 3) out;

layout (set = 0, binding = 0) uniform SHADOW 
{
	mat4 mvp[LIGHT_COUNT];
} shadow_data;

layout (location = 0) in int inInstanceIndex[];
void main() 
{
	for (int i = 0; i < 3; i++)
	{
		gl_Layer = gl_InvocationID;
		gl_Position = shadow_data.mvp[gl_InvocationID] * gl_in[i].gl_Position;;
		EmitVertex();
	}
	EndPrimitive();
}