const float PI = 3.1415926535897932384626433832795;

#define POINT_LIGHT 0
#define SPOT_LIGHT 1
#define DIRECTIONAL_LIGHT 2

struct Light
{
    vec3 pos;
    float cutoff;
    vec3 diffuse;
    float out_cutoff;
    vec3 dir;
    mat4 view_matrix;
    float falloff;
    int type; // 0 = point, 1 = spot, 2 = directional
};

layout(set = 0, binding = 1) uniform light_data {
    Light lights[20];
    int num_lights;
} light_ubo;

#define SHADOW_FACTOR 0.25
#define AMBIENT_LIGHT 0.1

layout(set = 2, binding = 0) uniform sampler2D viewPosBuffer;
layout(set = 2, binding = 1) uniform sampler2D normalBuffer;
layout(set = 2, binding = 2) uniform sampler2D albedoBuffer;
layout(set = 2, binding = 3) uniform sampler2D emissiveBuffer;
layout(set = 2, binding = 4) uniform sampler2D metalRoughnessAoBuffer;
layout(set = 2, binding = 5) uniform sampler2DArray shadowmap;

layout(location = 0) in vec2 vertexUV;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = clamp(dot(N, H), 0.0f, 1.0f);
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
    float NdotV = clamp(dot(N, V), 0.0f, 1.0f);
    float NdotL = clamp(dot(N, L), 0.0f, 1.0f);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// frag pos, normal are on the view space
vec4 SSR_raycast(vec4 view_pos, vec3 normal, vec2 tex_size, vec2 tex_coord) {
    vec4 uv = vec4(0.0f);

    vec4 vs_cam_to_pos = vec4(normalize(vec3(view_pos)), 0.0f);
    vec4 vs_reflect_vec = vec4(normalize(reflect(vs_cam_to_pos.xyz, normal)), 0.0f);

    vec4 vs_reflect_end_pos = view_pos + vs_reflect_vec * 1000;
    //if(vs_reflect_end_pos.z > 0)
    //    return uv;

    vs_reflect_end_pos /= (vs_reflect_end_pos.z > 0 ? vs_reflect_end_pos.z : 1);
    vec4 cs_reflect_end_pos = global_ubo.projection * vec4(vs_reflect_end_pos.xyz, 1.0f);
    cs_reflect_end_pos /= cs_reflect_end_pos.w;

    vec4 cs_pos = global_ubo.projection * view_pos;
    cs_pos /= cs_pos.w; // REVERSE DEPTH [1:0]

    //vec3 ts_pos_test_if_same;
    //ts_pos_test_if_same = vec3(cs_pos.xy * 0.5f + vec2(0.5f), texture(normalBuffer, tex_coord).w);
    
    vec3 ts_reflect_dir = normalize(cs_reflect_end_pos.xyz - cs_pos.xyz);
    ts_reflect_dir.xy *= 0.5f;

    vec3 ts_pos = vec3(cs_pos.xy * vec2(0.5f) + vec2(0.5f), cs_pos.z);
    vec3 ts_reflect_end_pos = vec3(cs_reflect_end_pos.xy * 0.5f + vec2(0.5f), cs_reflect_end_pos.z);

    float max_iter_count = (ts_reflect_dir.x >= 0 ? (1 - tex_coord.x) / ts_reflect_dir.x : -tex_coord.x / ts_reflect_dir.x);
    max_iter_count = min(max_iter_count, (ts_reflect_dir.y >= 0 ? (1 - tex_coord.y) / ts_reflect_dir.y : -tex_coord.y / ts_reflect_dir.y));
    max_iter_count = min(max_iter_count, (ts_reflect_dir.z >= 0 ? (1 - ts_pos.z) / ts_reflect_dir.z : -ts_pos.z / ts_reflect_dir.z));

    ts_reflect_end_pos = ts_pos + ts_reflect_dir * max_iter_count;

    vec3 ts_delta_pos = ts_reflect_end_pos - ts_pos;
    vec2 ss_pos = tex_size * ts_pos.xy;
    vec2 ss_reflect_end_pos = tex_size * ts_reflect_end_pos.xy;

    // not sure what this part do
    vec2 ss_delta_pos = ss_reflect_end_pos - ss_pos;
    int max_distance = int(max(max(abs(ss_delta_pos.x), abs(ss_delta_pos.y)), oout.min_iteration)); // * clamp(resolution, 0, 1);
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

        if(depth0 == 0.0f)
            break;

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

float textureProj(vec4 P, float layer, vec2 offset)
{
	float shadow = 1.0;
	vec4 shadowCoord = P / P.w;
	shadowCoord.st = shadowCoord.st * 0.5 + 0.5;
	
	if (shadowCoord.z > -1.0 && shadowCoord.z < 1.0) 
	{
		float dist = texture(shadowmap, vec3(shadowCoord.st + offset, layer)).r;
		if (shadowCoord.w > 0.0 && dist > shadowCoord.z) 
		{
			shadow = SHADOW_FACTOR;
		}
	}
	return shadow;
}

float filterPCF(vec4 sc, float layer)
{
	ivec2 texDim = textureSize(shadowmap, 0).xy;
	float scale = 1.5;
	float dx = scale * 1.0 / float(texDim.x);
	float dy = scale * 1.0 / float(texDim.y);

	float shadowFactor = 0.0;
	int count = 0;
	int range = 2;
	
	for (int x = -range; x <= range; x++)
	{
		for (int y = -range; y <= range; y++)
		{
			shadowFactor += textureProj(sc, layer, vec2(dx*x, dy*y));
			count++;
		}
	
	}
	return shadowFactor / count;
}


