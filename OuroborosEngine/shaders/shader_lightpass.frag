#version 450

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outUV;
layout(location = 1) in vec3 cam_pos;
layout(location = 2) in mat4 projection;

#include "common_light_pass_frag.glsl"

float test_att = 0.1;

void main()
{
    vec3 Lo = vec3(0);
    vec3 albedo = vec3(0.f);
    float ao = 0.f;
    float metallic = -1.f;
    float roughness = -1.f;

    albedo = texture(albedoBuffer, vertexUV).rgb;
    metallic = texture(metalRoughnessAoBuffer,vertexUV).r;
    roughness = texture(metalRoughnessAoBuffer,vertexUV).g;
    ao = texture(metalRoughnessAoBuffer,vertexUV).b;

    vec4 frag_pos = texture(viewPosBuffer, vertexUV);
    
    // SSR code start
    vec2 tex_size  = textureSize(viewPosBuffer, 0).xy;
    vec2 tex_coord = gl_FragCoord.xy / tex_size;

    vec4 uv = vec4(0.0f);

    SSR_raycast(uv, metallic, frag_pos, tex_size, tex_coord);

    for(int i = 0; i < light_ubo.num_lights; ++i) 
    {
        Light light = light_ubo.lights[i];
        vec3 L;
        float att;

        if(light.type == 0) {
            L = light.pos - frag_pos.xyz;
            float d = length(L);
            // att = 1 / (d * d);
            att = 1/(test_att*test_att);
            L = normalize(L);
        }
        else if(light.type == 1) 
        {
            L = light.pos - frag_pos.xyz;
            float d = length(L);
            att = 1/(test_att*test_att);
            L = normalize(L);
            
            float cutoff = radians(light.cutoff);
            float outcutoff = radians(light.out_cutoff);
            float alpha = dot(-L,normalize(light.dir)); 

            float Spotlighteffect=0;
            if(alpha < outcutoff)
            {
    	         Spotlighteffect=0;
            }
            else if(alpha < cutoff)
            {
    	        Spotlighteffect=1;
            }
            else
            {
    	        Spotlighteffect=pow((alpha-cos(outcutoff))/(cos(cutoff)-cos(outcutoff)),light.falloff);
         }
         att *= Spotlighteffect;
        }
        else if(light.type == 2) {
            L = -normalize(light.dir);
            att = 1;
        }

        vec3 radiance = att * light.diffuse;


        vec3 V = normalize(cam_pos - frag_pos.xyz);
        vec3 N = texture(normalBuffer, vertexUV).rgb;
        vec3 H = normalize(L + V);
        
        float D = DistributionGGX(N, H, roughness);

        vec3 F0 = vec3(0.04);
        F0 = mix(F0, albedo, metallic);
        vec3 F = FresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
        float G = GeometrySmith(N, V, L,roughness);

        vec3 Ks = F;
        vec3 Kd = vec3(1.0) - Ks;
        Kd *= 1.0 - metallic;

        vec3 specular = (D * F * G) / 4 * max(dot(V, N), 0.0) * max(dot(L, N), 0.0) + 0.0001f;
        Lo += (Kd * albedo / PI + specular) * radiance * max(dot(L, N), 0.0);
    }

    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;
    color = color /  (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    color += texture(emissiveBuffer,vertexUV).rgb;
    outColor = texture(viewPosBuffer, vertexUV);
    outUV = uv;
}

