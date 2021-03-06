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

layout(set = 2, binding = 0) uniform Material 
{
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
    int has_albedo_texture;
    int has_normal_texture;
    int has_metalroughness_texture;
    int has_ao_texture;
    int has_metalic_texture;
    int has_roughness_texture;
    int has_emissive_texture;
    int is_roughness_texture_inverted;
} material;

layout(set = 2, binding = 1) uniform sampler2D albedo_texture;
layout(set = 2, binding = 2) uniform sampler2D normal_texture;
layout(set = 2, binding = 3) uniform sampler2D metalroughness_texture;
layout(set = 2, binding = 4) uniform sampler2D ao_texture;
layout(set = 2, binding = 5) uniform sampler2D roughness_texture;
layout(set = 2, binding = 6) uniform sampler2D emissive_texture;

layout(location = 0) in VS_IN 
{
    vec3 norm;
    vec3 frag_pos;
    vec3 cam_pos;
    vec2 uv;
    vec3 non_pure_normal;
} vs_in;

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normal_texture,  vs_in.uv).xyz * 2.0 - 1.0;


    vec3 Q1  = dFdx(vs_in.frag_pos);
    vec3 Q2  = dFdy(vs_in.frag_pos);
    vec2 st1 = dFdx(vs_in.uv);
    vec2 st2 = dFdy(vs_in.uv);

    vec3 N   = normalize(vs_in.non_pure_normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize( tangentNormal);
}

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

vec3 AlbedoValue()
{
    vec3 result = vec3(0.f);
    if(material.has_albedo_texture != 0)
    {
        result = pow(texture(albedo_texture, vs_in.uv).xyz, vec3(2.2));
    }
    else
    {
        result = material.albedo;
    }
    return result;
}


float MetalicValue()
{
    float result = 0.f;
    if(material.has_metalroughness_texture != 0)
    {
        result = texture(metalroughness_texture, vs_in.uv).z;    
    }

    if(material.has_metalic_texture != 0)
    {
        result = texture(metalroughness_texture, vs_in.uv).r;
    }

    if(material.has_metalic_texture == 0 && material.has_metalroughness_texture == 0)
    {
        result = material.metallic;
    }

    return result;
} 

float RoughnessValue()
{
    float result = 0.f;
    if(material.has_metalroughness_texture != 0)
    {
        if(material.is_roughness_texture_inverted == 0)
        {
            result = texture(metalroughness_texture, vs_in.uv).y;
        }
        else
        {
            result = 1.0 - texture(metalroughness_texture, vs_in.uv).y;
        }
    }
   

    if(material.has_roughness_texture != 0)
    {
        if(material.is_roughness_texture_inverted == 0)
        {
          result = texture(roughness_texture, vs_in.uv).r;
        }
        else
        {
          result = 1.0 - texture(roughness_texture, vs_in.uv).r;
        }
    }

    if(material.has_roughness_texture == 0 && material.has_metalroughness_texture == 0 )
    {
        result = material.roughness;
    }
        
    return result;
}

vec3 EmissiveValue()
{
    vec3 result = vec3(0.f);
    result = texture(emissive_texture, vs_in.uv).rgb;
    return result;
}

float AoValue()
{
    float result = 0.f;
    if(material.has_ao_texture != 0)
    {
        result = texture(ao_texture, vs_in.uv).r;
    }
    else
    {
        result = material.ao;
    }
    return result;
}