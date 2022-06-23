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
     vec3 tex_coord;
    vec3 cam_pos;
} vs_in;

layout(set = 2, binding = 0) uniform Material 
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
} material;

layout(set = 2, binding = 1) uniform sampler2D texture;


vec3 DirLight(Light lig, vec3 normal, vec3 fragposition,vec3 viewDir)
{
    vec3 lightDir = normalize(-lig.direction);
    vec3 norm = normalize(normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 reflectDirection = 2*dot(norm, lightDir)*norm - lightDir;
    
    vec3 specular =vec3(0.0);
    float spec = pow(max(dot(viewDir, reflectDirection), 0.0), material.shininess*128);
    specular= lig.specular * spec * material.specular;

    vec3 ambient  = lig.ambient * material.ambient;
    vec3 diffuse  = lig.diffuse * diff * material.diffuse;
    return (ambient + diffuse + specular);
}


void main() 
{
//  vec3 viewDirection = normalize(vs_in.cam_pos - vs_in.frag_position);
//  vec3 result = vec3(0.0);
//   for(int i =0; i < light_ubo.num_lights; ++i)
//   {
//     result += DirLight(light_ubo.lights[i],vs_in.norm,vs_in.frag_position,viewDirection);
//   }
    // outColor = vec4(vs_in.norm, 1.0);
    outColor = vec4(vs_in.tex_coord, 1.0);
 
}