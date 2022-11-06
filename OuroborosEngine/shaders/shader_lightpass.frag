#version 450

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outUV;

layout(set = 0, binding = 0) uniform global_data {
    mat4 projection;
    mat4 view;
    vec3 cam_pos;
    mat4 inv_view;
} global_ubo;

#include "common_light_pass_frag.glsl"

float test_att = 0.1;

layout(set = 1, binding = 0) uniform Test {
    float att;
    float c1;
    float c2;
    float c3;
} oout;

float CalculateAttenuation(float c1, float c2, float c3, float dist)
{
    return min(1.f/(c1 + c2 * dist + c3 * dist * dist), 1.f);
}

void main()
{
    vec3 Lo = vec3(0);
   
    vec3 frag_pos = texture(posBuffer, vertexUV).rgb;
    vec3 normal = texture(normalBuffer,vertexUV).rgb;
    vec3 albedo = texture(albedoBuffer,vertexUV).rgb;
    float metallic = texture(metalRoughnessAoBuffer, vertexUV).r;
    float ao = texture(metalRoughnessAoBuffer, vertexUV).b;
    float roughness = texture(metalRoughnessAoBuffer, vertexUV).g;
      
    vec3 V = normalize(-frag_pos);
    vec3 N = texture(normalBuffer,vertexUV).rgb;
    
    vec4 uv = vec4(0.0f);
    if(metallic > 0.01f) 
    {
        vec4 view_pos = vec4(frag_pos, 1.0f);
        vec2 tex_size = textureSize(posBuffer, 0).xy;
        uv = SSR_raycast(uv, view_pos, N, tex_size, vertexUV);
    }

    for(int i = 0; i < light_ubo.num_lights; ++i) 
    {
        //Light informations
        Light light = light_ubo.lights[i];
        const vec3 light_pos    = light.pos;
        const vec3 light_dir    = normalize(light.dir);
        const int  light_type   = light.type;

        vec3 L; // light to fragment vector
        float att, d;

        if(light_type == 2){
            L = -normalize(light_dir);
            // att = 10;
        }
        else{
            vec3 relative_vec = light_pos - frag_pos;
            d = length(relative_vec);
            L = normalize(relative_vec);
            // att = 1 / (oout.att * oout.att);
        
            att =  CalculateAttenuation(oout.c1, oout.c2, oout.c3, d);
        }


        //Variables for for PBR
        vec3 H  = normalize(L + V);
        float D = DistributionGGX(N, H, roughness);

        vec3 F0 = vec3(0.04f);
        F0      = mix(F0, albedo, metallic);
        vec3 F  = FresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

        float NDF   = DistributionGGX(N, H, roughness);

        vec3 wi = normalize(light_pos - frag_pos);
        vec3 radiance = att * light.diffuse * max(dot(N, L), 0.f);

        switch(light_type)
        {
            case 0: // point light
            {
                break;
            }
            case 1: // spot light
            {
                const float cos_phi             = cos(radians(light.out_cutoff));
                const float cos_theta           = cos(radians(light.cutoff));
                
                float spot_light_effect   = 0.f;

                const float L_dot_D             = dot(L, light_dir);

                if(L_dot_D < cos_phi) {
                    spot_light_effect = 0.f;
                    // outColor = vec4(1.f, 0.f, 0.f, 1.f);
                    // return;
                }
                else if(L_dot_D > cos_theta) {
                    spot_light_effect = 1.f;
                    // outColor = vec4(0.f, 1.f, 0.f, 1.f);
                    // return;
                }
                else {
                    const float num = L_dot_D - cos_phi;
                    const float denom   = cos_theta - cos_phi;
                    spot_light_effect = pow(num / denom, light.falloff);
                    
                    // outColor = vec4(1 - spot_light_effect, spot_light_effect, 0.f, 1.f);
                    // return;
                }
                radiance *= spot_light_effect;
                break;
            }
            case 2: // directional light
            {
                radiance = light.diffuse ;
                break;
            }
        }

        radiance *= max(1.f, oout.att);

        float NdotL = dot(N, L);
        float G = GeometrySmith(N, V, L, roughness);

        vec3 num = NDF * G * F;
        float denom = 4.f * max((dot(N, V)), 0.f) * max(NdotL, 0.f) + 0.0001;
        vec3 specular = num / denom;

        // outColor = vec4(specular, 1.f );
        // return;

        vec3 Ks = F;
        vec3 Kd = vec3(1.f) - Ks;

        Kd *= 1.f - metallic;

        Lo += (Kd * albedo / PI + specular) * radiance * NdotL;
    }

    vec3 ambient = vec3(0.03) * albedo * ao * 0.1f;
    vec3 color = ambient + Lo;
    color = color /  (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    color += texture(emissiveBuffer, vertexUV).rgb;

    outColor = vec4(color, roughness);
    outUV = uv;
}
