#version 330 core

in vec3 vDirection;

uniform samplerCube tex;

out vec4 color;

void main()
{
    color = texture(tex, vDirection); //Cube textures are sampled using a 3D direction vector (vec3)
}