const float PI = 3.1415926535897932384626433832795;
const float step = 0.1;
const float minRayStep = 0.1;
const float maxSteps = 30;
const int numBinarySearchSteps = 5;

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

layout(set = 2, binding = 0) uniform sampler2D posBuffer;
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

vec3 view_to_screen(vec3 view_pos, vec2 tex_size) {

    vec4 clip_space = global_ubo.projection * vec4(view_pos, 1.0f);
    clip_space.xyz /= clip_space.w;
    vec3 screen_pos = vec3((clip_space.xy * 0.5f + 0.5f) * tex_size, 1.0f);
    screen_pos.z = texture(posBuffer, screen_pos.xy).z;

    return screen_pos;
}

// frag pos, normal are on the view space
void SSR_raycast(inout vec4 reflected_uv, vec4 frag_pos, vec3 normal, vec2 tex_size, vec2 tex_coord) {

    if(frag_pos.w <= 0.0f) 
    {   
        return;
    }

    vec3 ray_o = frag_pos.xyz;
    vec3 ray_v = normalize(ray_o); // camera to frag_pos
    vec3 ray_d = normalize(reflect(ray_v, normal));
    vec3 view_first_step = ray_o + ray_d;
    
    //convert ray origin to screen space
    vec3 ray_o_screen = view_to_screen(ray_o, tex_size);
    vec3 ray_step_screen = view_to_screen(view_first_step, tex_size);

    vec3 screen_step_dist = ray_step_screen - ray_o_screen; // depth in world / x,y in screen
    vec3 ray_d_screen = minRayStep * normalize(screen_step_dist);

    vec3 prev_pos = ray_o_screen;
    vec3 curr_pos = prev_pos + ray_d_screen;

    for(int i = 0; i < maxSteps; ++i)
    {
        // offscreen
        if(min(curr_pos,vec3(0)) != vec3(0) || max(curr_pos, vec3(1)) != vec3(1)) {
            return;
        }

        // check ray hit
        float diff = curr_pos.z - texture(posBuffer, curr_pos.xy).z;

        if(diff >= 0.0 && diff < length(ray_d_screen)) 
        {
            // binary search for refinement
            reflected_uv.xy = curr_pos.xy;
            return;
        }

        prev_pos = curr_pos;
        curr_pos = prev_pos + ray_d_screen;
    }
    // how far fragment can reflect
    // float max_distance;
    // how many fragments are skipped (0 = no refelection ~ 1)
    // float resolution;
    // second pass iteration (ray-marching?)
    // int steps;
    // cutoff between what counts as a possible reflection hit
    // float thickness;



}