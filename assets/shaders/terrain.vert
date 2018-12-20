#version 330 core

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 uv;

uniform mat4 MVP;
uniform sampler2D tex;
uniform vec2 tiling;

out vec2 vUV;
out float height;

void main()
{
    vUV = uv * tiling;
    height = texture(tex, uv).r; //the color sampled from the texture will be used as the local y value
    vec3 modified_pos = vec3(position.x, height, position.z);
    gl_Position = MVP * vec4(modified_pos, 1.0f);
}