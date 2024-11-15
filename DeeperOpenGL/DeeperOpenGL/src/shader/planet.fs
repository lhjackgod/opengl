#version 420 core

layout(location = 0) out vec4 FragColor;

in vec2 vTexCoord;

struct Texture
{
    sampler2D diffuse1;
};

uniform Texture material;

void main()
{
    FragColor = texture(material.diffuse1, vTexCoord);
}