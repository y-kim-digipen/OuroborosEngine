#version 450

#include "common_light_pass_frag.glsl"

layout(location = 0) out vec4 outColor;
layout(location = 1) in vec3 cam_pos;
float test_att = 0.1;

void main()
{
    vec3 Lo = vec3(0);
    vec3 albedo = vec3(0.f);
    float ao = 0.f;

    vec3 frag_pos = texture(viewPosBuffer, vertexUV).rgb;
    vec3 normal = texture(normalBuffer,vertexUV).rgb;
    
 for(int i = 0; i < light_ubo.num_lights; ++i) 
    {
        Light light = light_ubo.lights[i];
        vec3 L;
        float att;

        if(light.type == 0) {
            L = light.pos - frag_pos;
            float d = length(L);
            // att = 1 / (d * d);
            att = 1/(test_att*test_att);
            L = normalize(L);
        }
        else if(light.type == 1) 
        {
            L = light.pos - frag_pos;
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


        vec3 V = normalize(cam_pos - frag_pos);
        vec3 N = texture(normalBuffer, vertexUV).rgb;
        vec3 H = normalize(L + V);

        float metallic = 0.f;
        float roughness = 0.f;

        albedo = texture(albedoBuffer, vertexUV).rgb;
        metallic = texture(metalRoughnessAoBuffer,vertexUV).r;
        roughness = texture(metalRoughnessAoBuffer,vertexUV).g;
        ao = texture(metalRoughnessAoBuffer,vertexUV).b;
        
        
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



    outColor = vec4(color, 1.0);
}