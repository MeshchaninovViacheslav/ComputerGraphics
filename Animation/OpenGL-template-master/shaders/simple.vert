#version 320 es

precision mediump float;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform float time;

void main()
{
    float distance_from_flagpole = position.x + 0.5;

    gl_Position = vec4(position.x, sin(time + distance_from_flagpole) * 0.5 * distance_from_flagpole + position.y, position.z, 1.0);
    TexCoord = aTexCoord;
    //TexCoord = vec2(position.x + 0.5, position.y + 0.5);
}