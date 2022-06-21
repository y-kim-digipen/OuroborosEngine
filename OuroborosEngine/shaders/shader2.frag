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

layout(location = 0) in VS_IN 
{
    vec3 norm;
    vec3 frag_position;
     vec2 tex_coord;
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

    outColor = vec4(vs_in.tex_coord.x, vs_in.tex_coord.y, 0.5, 1.0);
}