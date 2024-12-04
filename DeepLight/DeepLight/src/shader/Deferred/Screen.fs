#version 420 core

layout(location = 0) out vec4 FragColor;

in vec2 vTexCoord;
uniform sampler2D uScreenTexture;
void main()
{
    vec4 resultColor = texture(uScreenTexture, vTexCoord);
    FragColor = resultColor;
}