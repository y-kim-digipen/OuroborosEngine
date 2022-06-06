#version 450

/*
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
*/

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform global_data 
{
    mat4 projection;
    mat4 view;
    vec3 cam_pos;
} global_ubo;

layout(location = 0) in VS_IN 
{
    vec3 norm;
    vec3 frag_position;
} vs_in;

layout(set = 2, binding = 0) uniform material 
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
} global_ubo;

void main() 
{
    vec3 viewDirection = normalize(global_ubo.cam_pos - vs_in.frag_position);
    //todo light dirrection add -lightdirection
    vec3 lightDir = normalize();
    vec3 norm = normalize(vs_in.norm);
    
    float diff = max(dot(norm, lightDir), 0,0);
    vec3 reflectDirection = 2*dot(norm, lighDir)*norm - lightDir;

    float spec = pow(max(dot(viewDir, reflectDirection), 0.0), global_ubo.shininess*128);

//todo light multiply light specular
    vec3 specular = spec * global_ubo.specular;
//todo light mulitply light ambient
    vec3 ambient = global_ubo.ambient;
//todo light mulitply light diffuse
    vec3 diffuse = global_ubo.diffuse * diff ;



    outColor = vec4(specular + ambient + diffuse);
}