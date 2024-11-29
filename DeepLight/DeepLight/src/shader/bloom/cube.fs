#version 420 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;
in VS_OUT
{
    vec3 vFragPos;
    vec2 vTexCoord;
    vec3 vNormal;
} fs_in;

struct Light
{
    vec3 position;
    vec3 color;
};

uniform sampler2D u_DiffuseTexture;
uniform Light lights[16];
uniform vec3 u_CameraPos;
uniform int u_LightCount;
void main()
{
    vec3 CameraDir = normalize(u_CameraPos - fs_in.vFragPos);

    vec3 normal = normalize(fs_in.vNormal);

    vec3 diffuseAlbedo = texture(u_DiffuseTexture, fs_in.vTexCoord).rgb;

    vec3 resultColor = vec3(0.0);
    for(int i = 0; i < u_LightCount; i++)
    {
        vec3 lightDir = normalize(lights[i].position - fs_in.vFragPos);
        vec3 diffuseColor = max(dot(normal, lightDir), 0.0) * diffuseAlbedo * lights[i].color;

        //attenuation
        float distance = length(lights[i].position - fs_in.vFragPos);

        float attenuation = 1.0 / (distance);

        diffuseColor *= attenuation;

        resultColor += diffuseColor;
    }
    float brightness = dot(resultColor, vec3(0.2126, 0.7152, 0.0722));
    FragColor = vec4(resultColor, 1.0);
    if(brightness > 1.0)
    {
        BrightColor = vec4(resultColor, 1.0);
    }
    else
    {
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}