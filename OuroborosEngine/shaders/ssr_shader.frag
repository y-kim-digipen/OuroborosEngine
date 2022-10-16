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
    vec3 reflected_color = texture(color_buffer, uv.xy).rgb;
    vec4 blurred_color = vec4(mix(vec3(0), reflected_color, uv.b), uv.b);

    outColor = mix(color, blurred_color, roughness);
}