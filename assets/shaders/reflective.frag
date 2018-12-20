#version 330 core

in vec3 vDirection;
in vec3 vNormal;

uniform samplerCube tex;
uniform vec4 tint;

out vec4 color;

void main()
{
    //reflect returns the reflected direction after it hits a surface with the given normal
    //Note: for refraction, use refract
    vec3 reflected = reflect(vDirection, normalize(vNormal));
    color = tint * texture(tex, reflected);
}