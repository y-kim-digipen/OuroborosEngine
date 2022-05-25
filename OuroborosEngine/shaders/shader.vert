#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;


void main() {
    gl_Position = vec4(pos, 1.0);
}