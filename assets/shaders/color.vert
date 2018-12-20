#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;

uniform mat4 MVP;

out vec4 vColor;

void main()
{
    vColor = color;
    gl_Position = MVP * vec4(position, 1.0f);
}