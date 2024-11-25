#version 420

layout(location = 0) out vec4 FragColor;

in vec2 vTexCoord;

uniform sampler2D uTexture;
void main()
{
    float depthValue = texture(uTexture, vTexCoord).r;
    FragColor = vec4(vec3(depthValue), 1.0);
}