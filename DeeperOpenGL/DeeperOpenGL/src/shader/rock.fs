#version 420 core
layout(location = 0) out vec4 FragColor;

struct Texture{
    sampler2D diffuse1;
};
uniform Texture material;
in vec2 vTexCoord;
void main()
{
    FragColor = texture(material.diffuse1, vTexCoord);
}