#version 420 core

layout(location = 0) out vec4 FragColor;

in vec2 vTexCoord;
uniform sampler2D ScreenTexture;
void main()
{
    FragColor = vec4(texture(ScreenTexture, vTexCoord).r);
}