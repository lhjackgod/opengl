#version 420 core

layout(location = 0) out vec4 FragColor;

uniform vec3 uLightColor;

void main()
{
    FragColor = vec4(uLightColor, 1.0);
}