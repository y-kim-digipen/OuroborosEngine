#version 450

/*
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
*/

struct Light
{
    vec3 position;
	float cutoff;
	vec3 diffuse;
	float out_cutoff;
	vec3 ambient;
	float falloff;
	vec3 specular;
    float padding;
	vec3 direction;
	int light_type;
};


layout(set = 0, binding = 1) uniform light_data {
    Light lights[20];
    int num_lights;
} light_ubo;

layout(location = 0) out vec4 outColor;

layout(location = 0) in VS_IN 
{
    vec3 norm;
    vec3 frag_position;
    vec3 cam_pos;
} vs_in;

layout(set = 2, binding = 0) uniform Material 
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
} material;

layout(set = 2, binding = 1) uniform sampler2D texture1;


#include "common.glsl"

// vec3 PointLight(Light lig, vec3 normal, vec3 fragposition, vec3 viewDir)
// {
//     vec3 lightDir =normalize(lig.position - fragposition);



// }

void main() 
{
 vec3 viewDirection = normalize(vs_in.cam_pos - vs_in.frag_position);
 vec3 result = vec3(0.0);
  for(int i =0; i < light_ubo.num_lights; ++i)
  {
    result += DirLight(light_ubo.lights[i],vs_in.norm,vs_in.frag_position,viewDirection);
  }

    outColor = vec4(result, 1.0);
    // if(light_ubo.num_lights > 0)
    // {
    //     outColor = vec4(light_ubo.lights[0].ambient, 1.0);
    // }
    // else
    // {
    //  outColor = vec4(material.ambient,1.0);
    // }
}