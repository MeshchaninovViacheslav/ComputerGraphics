#version 320 es

precision mediump float;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in float aLen;

out vec2 TexCoord;

uniform float time;
uniform vec3 offsets[100];

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    float distance_from_flagpole = aLen / 10. * 1.5;

    vec2 offset = vec2(offsets[gl_InstanceID].x, offsets[gl_InstanceID].z) + vec2(0.0, 0.1);

    gl_Position = vec4(position / 10. * 1.5, 1.0);
    gl_Position.y += sin(time + 25. * distance_from_flagpole) * 0.5 * distance_from_flagpole;
    gl_Position += vec4(offset * 1.5, 0., 0.);

    gl_Position = projection * view * model * gl_Position;

    TexCoord = aTexCoord;
}