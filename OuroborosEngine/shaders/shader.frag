#version 450

#include "common_frag.glsl"

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform Test {
    float att;
} oout;

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
            // att = 1 / (d * d);
            att = 1/(oout.att*oout.att);
            L = normalize(L);
        }
        else if(light.type == 1) {

         

            L = light.pos - vs_in.frag_pos;
            float d = length(L);
              att = 1/(oout.att*oout.att);
            L = normalize(L);
            // float theta = dot(L, normalize(-light.dir));
            // float epsilon = light.cutoff - light.out_cutoff;
            // att *= clamp((theta - light.out_cutoff) / epsilon, 0, 1);
        
        float cutoff = radians(light.cutoff) ;
        float outcutoff = radians(light.out_cutoff) ;
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

        vec3 V = normalize(vs_in.cam_pos - vs_in.frag_pos);
        vec3 N = normalize(vs_in.non_pure_normal);
        vec3 H = normalize(L + V);

        if(material.has_normal_texture != 0)
        {
            N = getNormalFromMap();
        }

     
		float metallic = 0.f;
        float roughness = 0.f;
      
        albedo = AlbedoValue();
        metallic = MetalicValue();
        roughness = RoughnessValue();
        ao = AoValue();
   
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

    if(material.has_emissive_texture != 0)
    {
        color += EmissiveValue();   
    }
    
 
    outColor = vec4(color, 1.0);
}