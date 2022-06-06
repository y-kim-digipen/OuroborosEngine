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

layout(location = 0) in VS_IN {
    vec3 norm;
} vs_in;

layout(set = 2, binding = 0) uniform material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
} global_ubo;

void main() {
    outColor = vec4(1.0,0,0, 1.0);
}