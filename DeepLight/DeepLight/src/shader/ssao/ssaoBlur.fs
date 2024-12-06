#version 420 core

layout(location = 0) out float ssaoRate;

in vec2 vTexCoord;

uniform sampler2D ssaoTexture;

void main()
{
    vec2 offset = 1.0 / (textureSize(ssaoTexture, 0));

    float result = 0.0;

    for(int i = -1; i <= 1; i++)
    {
        for(int j = -1; j <= 1; j++)
        {
            vec2 offsetuv = vec2(float(i), float(j)) * offset;
            result += texture(ssaoTexture, vTexCoord + offsetuv).r;
        }
    }
    ssaoRate = result / 9.0;
}