#version 450

layout(location = 0) in vec2 vertexUV;
layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D color_buffer;
layout(set = 1, binding = 1) uniform sampler2D uv_buffer;

void main()
{
    vec4 color = texture(color_buffer, vertexUV);
    float roughness = color.w;
    color.w = 1.0f;
    vec4 uv = texture(uv_buffer, vertexUV);
    vec4 reflected_color = vec4(texture(color_buffer, uv.xy).rgb, 1.0f);

    outColor = mix(color, reflected_color, roughness);
}