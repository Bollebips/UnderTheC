#version 330 core

out vec4 outColor;

uniform vec4 customColor;

void main()
{
    outColor = customColor;
}
