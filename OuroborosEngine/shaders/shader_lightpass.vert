#version 450

layout(location = 0) out vec2 vertexUV;
layout(location = 1) out vec3 cam_pos;
layout(location = 2) out mat4 projection; //TODO: use set=0 both in vert&frag
layout(location = 6) out mat4 view;
layout(set = 0, binding = 0) uniform global_data {
    mat4 projection;
    mat4 view;
    vec3 cam_pos;
} global_ubo;

void main()
{
    vertexUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
	gl_Position = vec4(vertexUV * 2.0f - 1.0f, 0.0f, 1.0f);
    cam_pos = global_ubo.cam_pos;
    projection = global_ubo.projection;
    view = global_ubo.view;
}
