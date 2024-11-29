#version 420 core

layout(location = 0) out vec4 FragColor;

in vec2 vTexCoord;

uniform sampler2D u_ResultTexture;
uniform sampler2D u_BloomTexture;
uniform bool u_bloom = true;
uniform float exposure;
void main()
{
    vec3 resultColor = texture(u_ResultTexture, vTexCoord).rgb;
    if(u_bloom)
    {
        resultColor += texture(u_BloomTexture, vTexCoord).rgb;
    }
    resultColor = vec3(1.0) - exp(-exposure * resultColor);
    FragColor = vec4(pow(resultColor, vec3(1.0 / 2.2)), 1.0);
}