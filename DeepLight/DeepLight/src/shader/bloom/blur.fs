#version 420 core

layout(location = 0) out vec4 FragColor;

uniform sampler2D u_ResultTexture;
uniform bool u_Horizontal;

uniform float u_Weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

in vec2 vTexCoord;
void main()
{
    vec3 resultColor = texture(u_ResultTexture, vTexCoord).rgb * u_Weight[0];
    vec2 texOffset = 1.0 / textureSize(u_ResultTexture, 0);
    if(u_Horizontal)
    {
        for(int i = 1; i < 5; i++)
        {
            resultColor += texture(u_ResultTexture, vTexCoord + vec2(texOffset.x * i, 0.0)).rgb * u_Weight[i];
            resultColor += texture(u_ResultTexture, vTexCoord - vec2(texOffset.x * i, 0.0)).rgb * u_Weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; i++)
        {
            resultColor += texture(u_ResultTexture, vTexCoord + vec2(0.0, texOffset.y * i)).rgb * u_Weight[i];
            resultColor += texture(u_ResultTexture, vTexCoord - vec2(0.0, texOffset.y * i)).rgb * u_Weight[i];
        }
    }
    FragColor = vec4(resultColor, 1.0);
}