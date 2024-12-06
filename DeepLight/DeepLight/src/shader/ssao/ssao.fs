#version 420 core

layout(location = 0) out float FragColor;
layout(location = 1) out vec3 debugColor;
in vec2 vTexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];

uniform mat4 view;
uniform mat4 perspective;

uniform float ScreenWidth;
uniform float ScreenHeight;

uniform int kernelSize;
uniform float radius;
uniform float bias;

void main()
{
    vec2 noiseScale = vec2(ScreenWidth / 4.0, ScreenHeight / 4.0);
    vec2 texCoords = vTexCoord * noiseScale;

    //get information
    vec3 texRandomVec = normalize(texture(texNoise, texCoords).xyz);
    vec3 FragPos = texture(gPosition, vTexCoord).xyz;
    vec3 Normal = normalize(texture(gNormal, vTexCoord).xyz);
    
    //create TBN
    vec3 T = normalize(texRandomVec - dot(texRandomVec, Normal) * Normal);
    vec3 B = cross(Normal, T);
    mat3 TBN = mat3(T, B, Normal);

    //base the local coordinate normal system to sample point then get the world coordinate by TBN
    //calculate the occulusion
    float occulusion = 0.0;
    for(int i = 0; i < kernelSize; i++)
    {
        vec3 samplePoint = TBN * samples[i];
        samplePoint = FragPos + samplePoint * radius;
        vec4 offset = perspective * vec4(samplePoint, 1.0);//view space to clip space
        offset /= offset.w;
        vec2 offsetuv = offset.xy * 0.5 + 0.5;
        float sampleDepth = texture(gPosition, offsetuv).z;
        
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(FragPos.z - sampleDepth));
        occulusion += (sampleDepth >= samplePoint.z + bias ? 1.0 : 0.0) * rangeCheck;
    }
    FragColor = 1.0 - (occulusion / kernelSize);
    debugColor = texture(texNoise, texCoords).xyz;
}