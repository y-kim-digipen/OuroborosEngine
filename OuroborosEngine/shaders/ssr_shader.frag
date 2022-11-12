#version 450

layout(location = 0) in vec2 vertexUV;
layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D color_buffer;
layout(set = 1, binding = 1) uniform sampler2D uv_buffer;

vec4 get_output_color(vec2 vertex_uv)
{
    vec4 color = texture(color_buffer, vertex_uv);
    vec4 reflected_uv = texture(uv_buffer, vertex_uv);

    if(reflected_uv.w == 0) {
        return color;
    }
    else {
        float roughness = reflected_uv.z;
        vec4 reflected_color = texture(color_buffer, reflected_uv.xy);
        return mix(color, reflected_color, reflected_uv.z);
    }
}

void main()
{
    vec4 color = texture(color_buffer, vertexUV);
    vec4 reflected_uv = texture(uv_buffer, vertexUV);
    float roughness = reflected_uv.z;
    vec4 reflected_color = vec4(texture(color_buffer, reflected_uv.xy).rgb, 1.0f);
    vec4 result = vec4(0.0f);

    if(reflected_uv.w == 0)
        result = color;
    else
    {
        result = mix(color, reflected_color, roughness);
        vec2 tex_size = 1.0f / textureSize(color_buffer, 0).xy;

        {
            vec2 offset = vec2(-1, 1) * tex_size;
            result += get_output_color(vertexUV + offset);
        }

        {
            vec2 offset = vec2(0, 1) * tex_size;
            result += get_output_color(vertexUV + offset);
        }

        {
            vec2 offset = vec2(1, 1) * tex_size;
            result += get_output_color(vertexUV + offset);
        }

        {
            vec2 offset = vec2(-1, 0) * tex_size;
            result += get_output_color(vertexUV + offset);
        }

        {
            vec2 offset = vec2(1, 0) * tex_size;

            result += get_output_color(vertexUV + offset);
        }

        {
            vec2 offset = vec2(-1, -1) * tex_size;
            result += get_output_color(vertexUV + offset);
        }

        {
            vec2 offset = vec2(0, -1) * tex_size;
            result += get_output_color(vertexUV + offset);
        }

        {
            vec2 offset = vec2(1, -1) * tex_size;
            result += get_output_color(vertexUV + offset);
        }

        result /= 9.0f;
    }

    outColor = result;
}

