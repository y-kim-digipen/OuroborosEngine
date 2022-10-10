const float PI = 3.1415926535897932384626433832795;

struct Light
{
    vec3 pos;
    float cutoff;
    vec3 diffuse;
    float out_cutoff;
    vec3 dir;
    float falloff;
    int type; // 0 = point, 1 = spot, 2 = directional
};

layout(set = 0, binding = 1) uniform light_data {
    Light lights[20];
    int num_lights;
} light_ubo;

layout(set = 1, binding = 0) uniform ssr_params {
    // how far fragment can reflect
    float max_distance;
    // how many fragments are skipped (0 = no refelection ~ 1)
    float resolution;
    // second pass iteration (ray-marching?)
    int steps;
    // cutoff between what counts as a possible reflection hit
    float thickness;
} ssr;

layout(set = 2, binding = 0) uniform sampler2D viewPosBuffer;
layout(set = 2, binding = 1) uniform sampler2D normalBuffer;
layout(set = 2, binding = 2) uniform sampler2D albedoBuffer;
layout(set = 2, binding = 3) uniform sampler2D emissiveBuffer;
layout(set = 2, binding = 4) uniform sampler2D metalRoughnessAoBuffer;


layout(location = 0) in vec2 vertexUV;



float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void SSR_raycast(out vec4 uv, float mask, vec4 frag_pos, vec2 tex_size, vec2 tex_coord) {

    if(frag_pos.w <= 0.0f || mask <= 0.001f) {
        return;
    }

    vec3 normal = texture(normalBuffer,vertexUV).rgb;
    vec3 unit_frag_pos = normalize(frag_pos.xyz); // vector from camera to fragment 
    vec3 pivot = normalize(reflect(unit_frag_pos, normal)); // refelcted vector
    
    vec4 geometry_pos = frag_pos;

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
    float delta = max(abs(delta_pixel.y), abs(delta_pixel.x)) * clamp(ssr.resolution, 0, 1);
    vec2 increment = delta_pixel / max(delta, 0.001f);
    
    float search0 = 0; // to remember last pos before hit any geometry (help refine hit point)
    float search1 = 0;

    int hit0 = 0; // intersection during first pass
    int hit1 = 0; // intersection during second pass

    float view_distance = start_view.z;
    float depth = ssr.thickness; // view distance difference between the current ray point and scene position (ray & point offset)
    
    vec2 frag  = start_frag.xy; // converted to pixel coord since sampling in view space potentially sample on the same pixels (efficiency)
    uv.xy = frag / tex_size; // uv coord

    // ray marching
    for(int i = 0; i < int(delta); ++i) {
        // first pass (search ray hit to any geometry)
        frag += increment;
        uv.xy = frag / tex_size;
        geometry_pos = texture(viewPosBuffer, uv.xy);
        search1 = use_x > 0 ? (frag.x - start_frag.x) / delta_pixel.x : (frag.y - start_frag.y) / delta_pixel.y;
        //search1 = mix( (frag.y - startFrag.y) / deltaY, (frag.x - startFrag.x) / deltaX, useX); (same as above)
        
        // depth interpolation
        view_distance = (start_view.z * end_view.z) / mix(end_view.z, start_view.z, search1);
        depth = view_distance - geometry_pos.z;

        // check view_pos.depth of ray's end point matches geometry_pos.depth(any geometry viewpoint thats written into viewpos_buffer)
        if(depth > 0 && depth < ssr.thickness)
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
    int second_step = ssr.steps * hit0;
        
    for(int i = 0; i < second_step; ++i) {
        frag       = mix(start_frag.xy, end_frag.xy, search1);
        uv.xy      = frag / tex_size;
        geometry_pos = texture(viewPosBuffer, uv.xy);
        view_distance = (start_view.z * end_view.z) / mix(end_view.z, start_view.z, search1);
        if (depth > 0 && depth < ssr.thickness) {
            hit1 = 1;
            search1 = search0 + ((search1 - search0) * 0.5f);
        } else {
            float temp = search1;
            search1 = search1 + ((search1 - search0) * 0.5f);
            search0 = temp;
        } 
    }

    float visibility = 0.0f;
    if(uv.x > 0 && uv.x < 1 && uv.y > 0 && uv.y < 1) {
        visibility = hit1 * geometry_pos.w * ( 1 - max(dot(-unit_frag_pos, pivot), 0))* ( 1 - clamp( depth / ssr.thickness, 0, 1))
        * ( 1 - clamp( length(geometry_pos - frag_pos) / ssr.max_distance, 0, 1));
    }
    visibility = clamp(visibility, 0, 1);
    uv.ba = vec2(visibility);
}