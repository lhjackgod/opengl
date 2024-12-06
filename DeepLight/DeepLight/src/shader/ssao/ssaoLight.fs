#version 420 core
layout(location = 0) out vec4 FragColor;
in vec2 vTexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform sampler2D ssaoRate;
uniform sampler2D ssaoDebug;
struct Light{
    vec3 Position;
    vec3 Color;

    float linear;
    float quadratic;
};
uniform vec3 uCameraPos;
uniform Light light;
void main()
{
    vec3 FragPos = texture(gPosition, vTexCoord).rgb;
    vec3 Normal = texture(gNormal, vTexCoord).rgb;
    vec3 albedo = texture(gAlbedoSpec, vTexCoord).rgb;
   
    float ssao = texture(ssaoRate, vTexCoord).r;

    //lightDir
    vec3 lightDir = normalize(light.Position - FragPos);
    //cameraDir
    vec3 cameraDir = normalize(uCameraPos - FragPos);
    //halfway
    vec3 halfway = normalize(lightDir + cameraDir);
    //diffuse
    vec3 diffuseColor = max(0.0, dot(Normal, lightDir)) * albedo * light.Color;

    float distance = length(light.Position - FragPos);
    float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * (distance * distance));

    vec3 diffuse = diffuseColor * attenuation;
    vec3 ambient = albedo * vec3(0.3) * ssao;
    //specular
    vec3 specular = pow(max(0.0, dot(halfway, Normal)), 8.0f) * vec3(1.0) * attenuation;
    FragColor = vec4(diffuse + ambient, 1.0);
}