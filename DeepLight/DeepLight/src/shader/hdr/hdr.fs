#version 420 core

layout(location = 0) out vec4 FragColor;


uniform sampler2D hdrBuffer;
uniform float exposure;
in vec2 vTexCoord;
void main()
{
    vec3 resultColor = texture(hdrBuffer, vTexCoord).rgb;
    resultColor = pow(resultColor, vec3(1.0/ 2.2));

    resultColor = vec3(1.0) - exp(-exposure * resultColor);
    FragColor = vec4(resultColor, 1.0);
}