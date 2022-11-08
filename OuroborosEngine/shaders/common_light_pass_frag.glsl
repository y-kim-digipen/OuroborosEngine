const float PI = 3.1415926535897932384626433832795;

// light info (position, direction would be in view space)
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
vec4 SSR_raycast(vec4 view_pos, vec3 normal, vec2 tex_size, vec2 tex_coord) {
    
    vec4 clip_pos = global_ubo.projection * view_pos;
    clip_pos /= clip_pos.w; // perspective division of fragment

    // view space: camera to the view_pos
    vec3 cam_to_pos = normalize(vec3(view_pos));
    vec4 reflected_vec = vec4(reflect(cam_to_pos, normal), 0);
    vec4 reflected_end_pos =  view_pos + reflected_vec * 1000;   
    reflected_end_pos /= (reflected_end_pos.z < 0.0f ? reflected_end_pos.z : 1);
    vec4 cs_reflected_end_pos = global_ubo.projection * reflected_end_pos;
    cs_reflected_end_pos /= cs_reflected_end_pos.w; // perspective division reflected_end_pos
    vec3 reflected_dir = normalize(vec3(cs_reflected_end_pos - clip_pos));

    clip_pos.xy = clip_pos.xy * vec2(0.5f) + vec2(0.5f); // NDC space
    reflected_dir.xy *= vec2(0.5f);

    vec3 ts_pos = clip_pos.xyz;
    vec3 ts_reflected_dir = reflected_dir;
    float max_distance = ts_reflected_dir.x >= 0 ? (1 - ts_pos.x) / ts_reflected_dir.x : -ts_pos.x / ts_reflected_dir.x;
    max_distance = min(max_distance, ts_reflected_dir.y >= 0 ? (1 - ts_pos.y) / ts_reflected_dir.y : -ts_pos.y / ts_reflected_dir.y); 
    max_distance = min(max_distance, ts_reflected_dir.z >=0 ? (1 - ts_pos.z) / ts_reflected_dir.z : -ts_pos.z / ts_reflected_dir.z);

    // Find Intersection
    vec3 ts_reflected_end_pos = ts_pos + ts_reflected_dir * max_distance;
    vec3 dp = ts_reflected_end_pos - ts_pos;
    vec2 ss_pos = ts_pos.xy * tex_size;
    vec2 ss_reflected_end_pos = ts_reflected_end_pos.xy * tex_size;
    vec2 dp2 = ss_reflected_end_pos - ss_pos;
    max_distance = max(abs(dp2.x), abs(dp2.y));
    dp /= max_distance;

    vec4 ts_ray_pos = vec4( ts_pos + dp, 0 );
    vec4 ts_ray_dir_dt = vec4(dp, 0);
    vec4 ray_start_pos = ts_ray_pos;

    int hit_index = -1;

    for(int i = 0; i <= max_distance; i += 4)
    {
        vec4 ts_ray_pos0 = ts_ray_pos;
        vec4 ts_ray_pos1 = ts_ray_pos + ts_ray_dir_dt;
        vec4 ts_ray_pos2 = ts_ray_pos + ts_ray_dir_dt * 2;
        vec4 ts_ray_pos3 = ts_ray_pos + ts_ray_dir_dt * 3;

        //float depth0 = texture();
        float depth1 = 0;
        float depth2 = 0;
        float depth3 = 0;


        //float thickness = ts_ray_pos3 - 

    }


    return vec4(0.0f);
}