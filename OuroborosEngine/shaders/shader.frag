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
} vs_in;

layout(set = 2, binding = 0) uniform Material 
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
} material;

void main() 
{
//     vec3 viewDirection = normalize(global_ubo.cam_pos - vs_in.frag_position);
//     //todo light dirrection add -lightdirection
//     vec3 lightDir = normalize();
//     vec3 norm = normalize(vs_in.norm);
    
//     float diff = max(dot(norm, lightDir), 0,0);
//     vec3 reflectDirection = 2*dot(norm, lighDir)*norm - lightDir;

//     float spec = pow(max(dot(viewDir, reflectDirection), 0.0), material.shininess*128);

// //todo light multiply light specular
//     vec3 specular = spec * material.specular;
// //todo light mulitply light ambient
//     vec3 ambient = material.ambient;
// //todo light mulitply light diffuse
//     vec3 diffuse = material.diffuse * diff ;

    if(light_ubo.num_lights > 0)
    {
        outColor = vec4(light_ubo.lights[0].ambient, 1.0);
    }
    else
    {
     outColor = vec4(material.ambient,1.0);
    }
}