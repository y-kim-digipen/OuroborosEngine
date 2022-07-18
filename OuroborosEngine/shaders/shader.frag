#version 450

#include "common_frag.glsl"

layout(location = 0) out vec4 outColor;

layout(location = 0) in VS_IN 
{
    vec3 norm;
    vec3 frag_pos;
    vec3 cam_pos;
    vec2 uv;
    vec3 pure_normal;
} vs_in;



void main() 
{
    vec3 Lo = vec3(0);
    vec3 albedo = vec3(0.f);
    float ao = 0.f;

    for(int i = 0; i < light_ubo.num_lights; ++i) 
    {
        Light light = light_ubo.lights[i];
        vec3 L;
        float att;

        if(light.type == 0) {
            L = light.pos - vs_in.frag_pos;
            float d = length(L);
            att = 1 / (d * d);
            L = normalize(L);
        }
        else if(light.type == 1) {
            L = light.pos - vs_in.frag_pos;
            float d = length(L);
            att = 1 / (d * d);
            L = normalize(L);
            float theta = dot(L, normalize(-light.dir));
            float epsilon = light.cutoff - light.out_cutoff;
            att *= clamp((theta - light.out_cutoff) / epsilon, 0, 1);
        }
        else if(light.type == 2) {
            L = -normalize(light.dir);
            att = 1;
        }

        vec3 radiance = att * light.diffuse;

        vec3 V = normalize(vs_in.cam_pos - vs_in.frag_pos);
        vec3 N = normalize(vs_in.norm);
        vec3 H = normalize(L + V);

        vec3 NB = normalize(vs_in.pure_normal);

        mat3 tbn = mat3(NB, cross(N,NB), N);
        if(material.has_normal_texture != 0)
        {
            N = tbn *(texture(normal_texture, vs_in.uv).xyz * 2.0f - 1.0f);
        }

     
        float metallic = 0.f;
        float roughness = 0.f;
      
        if(material.has_albedo_texture != 0)
        {
            albedo = texture(albedo_texture, vs_in.uv).xyz;
        }
        else
        {
            albedo = material.albedo;
        }
   
        if(material.has_metalroughness_texture != 0)
        {
            metallic = texture(metalroughness_texture, vs_in.uv).z;
            roughness = texture(metalroughness_texture, vs_in.uv).y;
        }
        else
        {
            metallic = material.metallic;
            roughness = material.roughness;
        }
        if(material.has_ao_texture != 0)
        {
            ao = texture(ao_texture, vs_in.uv).r;
        }
        else
        {
            ao = material.ao;
        }

        
        float D = DistributionGGX(N, H, roughness);

        vec3 F0 = vec3(0.04);
        F0 = mix(F0, albedo, metallic);
        vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
        float G = GeometrySmith(N, V, L,roughness);

        vec3 Ks = F;
        vec3 Kd = vec3(1.0) - Ks;
        Kd *= 1.0 - metallic;

        vec3 specular = (D * F * G) / (4*  max(dot(V, N), 0.0) * max(dot(L, N), 0.0)+ 0.0001);
        Lo += (Kd * albedo / PI + specular) * radiance * max(dot(N, L), 0.0);
    }

    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;
    color = color /  (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    outColor = vec4(color, 1.0);
}