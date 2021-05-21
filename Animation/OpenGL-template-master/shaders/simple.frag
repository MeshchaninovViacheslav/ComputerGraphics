#version 320 es

precision mediump float;

out vec4 FragColor;

in vec2 TexCoord;

uniform vec3 color;
uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoord);
}