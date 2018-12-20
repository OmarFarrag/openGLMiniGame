#version 330 core

layout(location = 0) in vec3 position;
layout(location = 3) in vec3 normal;

uniform mat4 M;
uniform mat4 M_it;
uniform mat4 VP;
uniform vec3 cam_pos;

out vec3 vDirection;
out vec3 vNormal;

void main()
{
    vec4 world = M * vec4(position, 1.0f);
    vDirection = world.xyz - cam_pos;
    vNormal = normalize(M_it * vec4(normal, 0.0f)).xyz;
    gl_Position = VP * world;
}