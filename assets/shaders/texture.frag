#version 330 core

in vec2 vUV;

uniform sampler2D tex;

out vec4 color;

void main()
{
    //function texture take a "sampler" and "uv" then return a vec4 containing the sampled color
    color = texture(tex, vUV);
}