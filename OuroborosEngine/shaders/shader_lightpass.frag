#version 450

#include "common_light_pass_frag.glsl"

layout(location = 0) out vec4 outColor;
layout(location = 1) in vec3 cam_pos;
layout(location = 2) in mat4 projection;

float test_att = 0.1;

layout(set = 1, binding = 0) uniform ssr_params {
    // how far fragment can reflect
    float max_distance = 15.0f;
    // how many fragments are skipped (0 = no refelection ~ 1)
    float resolution = 0.3;
    // second pass iteration (ray-marching?)
    int steps = 10;
    // cutoff between what counts as a possible reflection hit
    float thickness = 0.5;
} ssr;

void ssr_raycast(out vec4 uv, float mask, vec4 frag_pos, vec2 tex_size, vec2 tex_coord);

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

    vec3 frag_pos = texture(viewPosBuffer, vertexUV).rgb;
    
    // SSR code start
    vec2 tex_size  = textureSize(viewPosBuffer, 0).xy;
    vec2 tex_coord = gl_FragCoord.xy / texSize;

    vec4 uv = vec4(0.0f);

    ssr_raycast(uv, metallic, frag_pos, tex_size, tex_coord);



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
    // color 
    // uv
    // no specular map (might need to use metallic & roughness)
    vec4 blurred_color = vec3(mix(vec3(0), color, uv.b), ub.b);
    //outColor = mix(outColor, blurred_color, roughness);
    outColor = blurred_color;
}

void ssr_raycast(out vec4 uv, float mask, vec4 frag_pos, vec2 tex_size, vec2 tex_coord) {

    if(frag_pos.w <= 0.0f || mask <= 0.001f) {
        return;
    }

    vec3 normal = texture(normalBuffer,vertexUV).rgb;
    vec3 unit_frag_pos = normalize(frag_pos.xyz); // vector from camera to fragment 
    vec3 pivot = normalize(reflect(unit_frag_pos, normal)); // refelcted vector

    vec3 geometry_pos = frag_pos;

    vec4 start_view = vec4(frag_pos.xyz, 1); // point where fragment starts
    vec4 end_view   = vec4(frag_pos.xyz + (pivot * ssr.max_distance), 1); // max point for ray to iterate

    vec4 start_frag = start_view;
    // Project to screen space.
    start_frag = projection * start_frag;
    // Perform the perspective divide.
    start_frag.xyz /= start_frag.w;
    // Convert the screen-space XY coordinates to UV coordinates.
    start_frag.xy = start_frag.xy * 0.5 + 0.5;

    // Convert the UV coordinates to fragment/pixel coordnates.
    start_frag.xy *= tex_size;

    vec4 end_frag = end_view;
    end_frag = projection * end_frag;
   	end_frag.xyz /= end_frag.w;
   	end_frag.xy = end_frag.xy * 0.5 + 0.5;
   	end_frag.xy *= tex_size;

    vec2 delta_pixel = end_frag.xy - start_frag.xy;
    float use_x = abs(delta_pixel.x) > abs(delta_pixel.y) ? 1 : 0; 
    float delta = max(abs(delta_pixel.y), abs(delta_pixel.x)) * clamp(resolution, 0, 1);
    vec2 increment = delta_pixel / max(delta, 0.001f);
    
    float search0 = 0; // to remember last pos before hit any geometry (help refine hit point)
    float search1 = 0;

    float hit0 = 0; // intersection during first pass
    float hit1 = 0; // intersection during second pass

    float view_distance = start_view.z;
    float depth = ssr.thickness; // view distance difference between the current ray point and scene position (ray & point offset)
    
    vec2 frag  = start_frag.xy; // converted to pixel coord since sampling in view space potentially sample on the same pixels (efficiency)
    uv.xy = frag / tex_size; // uv coord

    // ray marching
    for(int i = 0; i < int(delta); ++i) {
        // first pass (search ray hit to any geometry)
        frag += increment;
        uv = frag / tex_size;
        geometry_pos = texture(viewPosBuffer, uv).xyz;
        search1 = use_x > 0 ? (frag.x - start_frag.x) / delta_pixel.x : (frag.y - start_frag.y) / delta_pixel.y;
        //search1 = mix( (frag.y - startFrag.y) / deltaY, (frag.x - startFrag.x) / deltaX, useX); (same as above)
        
        // depth interpolation
        view_distance = (start_view.z * end_view.z) / mix(end_view.z, start_view.z, search1);
        depth = view_distance - geometry_pos.z;

        // check view_pos.depth of ray's end point matches geometry_pos.depth(any geometry viewpoint thats written into viewpos_buffer)
        if(depth > 0 && depth < thickness)
        {
            hit0 = 1;
            break;
        }
        else {
            search0 = search1;
        }
    }

    // binary search
    search1 = search0 + (search1 - search0) * 0.5f;
    int second_step = steps * hit0;
        
    for(int i = 0; i < second_step; ++i) {
        frag       = mix(start_frag.xy, end_frag.xy, search1);
        uv      = frag / tex_size;
        geometry_pos = texture(viewPosBuffer, uv).xyz;
        view_distance = (start_view.z * end_view.z) / mix(end_view.z, start_view.z, search1);
        if (depth > 0 && depth < thickness) {
            hit1 = 1;
            search1 = search0 + ((search1 - search0) * 0.5f);
        } else {
            float temp = search1;
            search1 = search1 + ((search1 - search0) * 0.5f);
            search0 = temp;
        } 
    }

    float visibility = hit1 * geometry_pos.w
    * ( 1
      - max
         ( dot(-unit_frag_pos, pivot)
         , 0
         )
      )
    * ( 1
      - clamp
          ( depth / ssr.thickness
          , 0
          , 1
          )
      )
    * ( 1
      - clamp
          (   length(geometry_pos - frag_pos)
            / ssr.max_distance
          , 0
          , 1
          )
      )
    * (uv.x < 0 || uv.x > 1 ? 0 : 1)
    * (uv.y < 0 || uv.y > 1 ? 0 : 1);

    visibility = clamp(visibility, 0, 1);
    uv.ba = vec2(visibility);
}

