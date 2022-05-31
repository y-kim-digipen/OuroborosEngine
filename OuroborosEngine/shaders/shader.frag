#version 450

layout(location = 0) out vec4 outColor;

layout(location = 0) in VS_IN {
    vec3 norm;
} vs_in;

void main() {
    outColor = vec4(1.0,0,0, 1.0);
}