#version 330 core

in vec2 vUV;
in float height;

uniform sampler2D tex;
uniform sampler2D bottom;
uniform sampler2D top;

out vec4 color;

void main()
{
    //based on height value mix between colors from 2 textures
    //using smoothstep we define when the transition will happen
    //in this case, below 0.2 is bottom and above 0.6 is top and in-between will be blended
    vec4 terrainColor = mix(texture(bottom, vUV), texture(top, vUV), smoothstep(0.2f, 0.6f, height));
    color = terrainColor;
}