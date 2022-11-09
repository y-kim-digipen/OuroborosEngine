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
    vec4 uv = vec4(0.0f);

    vec4 vs_cam_to_pos = vec4(normalize(vec3(view_pos)), 0.0f);
    vec4 vs_reflect_vec = vec4(normalize(reflect(vs_cam_to_pos.xyz, normal)), 0.0f);

    vec4 vs_reflect_end_pos = view_pos + vs_reflect_vec * 1000;
    vs_reflect_end_pos /= (vs_reflect_end_pos.z < 0 ? vs_reflect_end_pos.z : 1); // What is this?

    vec4 cs_reflect_end_pos = global_ubo.projection * vec4(vs_reflect_end_pos.xyz, 1.0f);
    cs_reflect_end_pos /= cs_reflect_end_pos.w;

    vec4 cs_pos = global_ubo.projection * view_pos;
    cs_pos /= cs_pos.w;

    vec3 ts_pos_test_if_same;
    ts_pos_test_if_same = vec3(cs_pos.xy * 0.5f + vec2(0.5f), texture(normalBuffer, tex_coord).w);
    
    vec3 ts_pos = vec3(tex_coord, texture(normalBuffer, tex_coord).w);
    vec3 ts_reflect_end_pos = vec3(cs_reflect_end_pos.xy * 0.5f + vec2(0.5f), cs_reflect_end_pos.z);

    vec3 ts_reflect_dir = normalize(ts_reflect_end_pos - ts_pos);

    //int max_iter_count = int(ts_reflect_dir.x >= 0 ? (1 - tex_coord.x) / ts_reflect_dir.x : -tex_coord.x / ts_reflect_dir.x);
    //max_iter_count = min(max_iter_count, int(ts_reflect_dir.y >= 0 ? (1 - tex_coord.y) / ts_reflect_dir.y : -tex_coord.y / ts_reflect_dir.y));
    //max_iter_count = min(max_iter_count, int(ts_reflect_dir.z >= 0 ? (1 - ts_pos.z) / ts_reflect_dir.z : -ts_pos.z / ts_reflect_dir.z));

    //ts_reflect_end_pos = ts_pos + ts_reflect_dir * max_iter_count;

    if(ts_reflect_dir.x > 0)
    {
        float y = (ts_reflect_dir.y / ts_reflect_dir.x) * (1 - ts_pos.x) + ts_pos.y;
        if(y >= 0 && y <= 1)
        {
            ts_reflect_end_pos.y = y;
            ts_reflect_end_pos.x = 1.0f;
        }
        
    } 
    else if(ts_reflect_dir.x < 0)
    {
        float y = (ts_reflect_dir.y / ts_reflect_dir.x) * -ts_pos.x + ts_pos.y;
        if(y >= 0 && y <= 1)
        {
            ts_reflect_end_pos.y = y;
            ts_reflect_end_pos.x = 0.0f;
        }
    }

    if(ts_reflect_dir.y > 0)
    {
        float x = (ts_reflect_dir.x / ts_reflect_dir.y) * (1 - ts_pos.y) + ts_pos.x;
        if(x >= 0 && x <= 1)
        {
            ts_reflect_end_pos.x = x;
            ts_reflect_end_pos.y = 1.0f;
        }
    }
    else if(ts_reflect_dir.y < 0)
    {
        float x = (ts_reflect_dir.x / ts_reflect_dir.y) * -ts_pos.y + ts_pos.x;
        if(x >= 0 && x <= 1)
        {
            ts_reflect_end_pos.x = x;
            ts_reflect_end_pos.y = 0.0f;
        }
    }

    vec3 ts_delta_pos = ts_reflect_end_pos - ts_pos;
    vec2 ss_pos = tex_size * tex_coord;
    vec2 ss_reflect_end_pos = tex_size * ts_reflect_end_pos.xy;

    // not sure what this part do
    vec2 ss_delta_pos = ss_reflect_end_pos - ss_pos;
    int max_distance = int(max(abs(ss_delta_pos.x), abs(ss_delta_pos.y)));
    ts_delta_pos /= max_distance;

    vec4 ts_ray_pos = vec4(ts_pos, 0);
    vec4 ts_ray_dir = vec4(ts_delta_pos, 0);

    int hit_index = -1;

    for(int i = 0; i < max_distance && i < oout.max_iteration; i += 4)
    {
        vec4 ts_ray_pos0 = ts_ray_pos + ts_ray_dir;
        vec4 ts_ray_pos1 = ts_ray_pos + ts_ray_dir * 2;
        vec4 ts_ray_pos2 = ts_ray_pos + ts_ray_dir * 3;
        vec4 ts_ray_pos3 = ts_ray_pos + ts_ray_dir * 4;

        float depth0 = texture(normalBuffer, ts_ray_pos0.xy).w;
        float depth1 = texture(normalBuffer, ts_ray_pos1.xy).w;
        float depth2 = texture(normalBuffer, ts_ray_pos2.xy).w;
        float depth3 = texture(normalBuffer, ts_ray_pos3.xy).w;

        {
            float thickness = ts_ray_pos3.z - depth3;
            hit_index = (thickness >= 0 && thickness < oout.max_thickness) ? (i + 4) : hit_index;
        }
        {
            float thickness = ts_ray_pos2.z - depth2;
            hit_index = (thickness >= 0 && thickness < oout.max_thickness) ? (i + 3) : hit_index;
        }
        {
            float thickness = ts_ray_pos1.z - depth1;
            hit_index = (thickness >= 0 && thickness < oout.max_thickness) ? (i + 2) : hit_index;
        }
        {
            float thickness = ts_ray_pos0.z - depth0;
            hit_index = (thickness >= 0 && thickness < oout.max_thickness) ? (i + 1) : hit_index;
        }


        ts_ray_pos = ts_ray_pos3;

        if(hit_index != -1) break;
    }

    if(hit_index != -1)
    {
        uv.w = 1.0f;
        uv.xy = ts_pos.xy + ts_ray_dir.xy * hit_index;
    }

    return uv;
} 