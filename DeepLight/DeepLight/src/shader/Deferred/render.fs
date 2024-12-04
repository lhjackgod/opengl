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
    float Klinear;
    float Kquadratic;
    float Kc;
};
uniform Light uLight[32];
uniform vec3 uViewPos;
uniform int uLightCount;

float getRadius(float distance, vec3 lightBright, Light light)
{
    float brightness = dot(lightBright, vec3(0.2126, 0.7152, 0.0722));
    float border = 5.0 / 256.0;
    float radius;
    float Molecule = border * border * light.Klinear * light.Klinear
    - 4.0 * border * light.Kquadratic * (border * light.Kc - brightness);
    if(Molecule < 0.0) return 0.0;
    radius = (-border * light.Klinear + sqrt(Molecule) / (2.0 * border * light.Kquadratic));
    if(radius < 0.0) return 0.0;
    return radius;
}

void main()
{
    vec3 objPos = texture(gPosition, vTexCoord).rgb;
    vec3 normal = texture(gNormal, vTexCoord).rgb;
    vec3 albedo = texture(gAlbedoSpec, vTexCoord).rgb;

    vec3 resultColor = vec3(0.0);
    for(int i = 0; i < uLightCount; i++)
    {
        float distance = length(uLight[i].Position - objPos);
        float radius = getRadius(distance, uLight[i].Color, uLight[i]);
        if(distance < radius)
        {
            vec3 LightDir = normalize(uLight[i].Position - objPos);
            vec3 diffColor = max(dot(normal, LightDir), 0.0) * albedo * uLight[i].Color;
            float attenuation = 1.0 / distance;
            resultColor += diffColor * attenuation;
        }
    }
    FragColor = vec4(resultColor, 1.0);
}