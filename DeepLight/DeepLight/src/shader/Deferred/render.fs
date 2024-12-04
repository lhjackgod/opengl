#version 420 core

layout(location = 0) out vec4 FragColor;

in vec2 vTexCoord;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light
{
    vec3 Position;
    vec3 Color;
};
uniform Light uLight[32];
uniform vec3 uViewPos;
uniform int uLightCount;

void main()
{
    vec3 objPos = texture(gPosition, vTexCoord).rgb;
    vec3 normal = texture(gNormal, vTexCoord).rgb;
    vec3 albedo = texture(gAlbedoSpec, vTexCoord).rgb;

    vec3 resultColor = vec3(0.0);
    for(int i = 0; i < uLightCount; i++)
    {
        vec3 LightDir = normalize(uLight[i].Position - objPos);
        vec3 diffColor = max(dot(normal, LightDir), 0.0) * albedo * uLight[i].Color;
        float distance = length(uLight[i].Position - objPos);
        float attenuation = 1.0 / distance;
        resultColor += diffColor * attenuation;
    }
    FragColor = vec4(resultColor, 1.0);
}