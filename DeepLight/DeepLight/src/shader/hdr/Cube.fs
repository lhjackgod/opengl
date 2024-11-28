#version 420 core
layout (location = 0) out vec4 FragColor;

in VS_OUT
{
    vec3 vFragPos;
    vec2 vTexCoord;
    vec3 vNormal;
} fs_in;

uniform sampler2D u_DiffuseTexture;

uniform vec3 uCameraPos;

uniform int uLightNums;
uniform vec3 uLightPosArray[16];
uniform vec3 uLightPosColorArray[16];

void main()
{
    vec3 resultColor = vec3(0.0);

    //cameraDir
    vec3 cameraDir = normalize(uCameraPos - fs_in.vFragPos);
    vec3 normal = normalize(fs_in.vNormal);

    vec3 diffuseAlbedo = texture(u_DiffuseTexture, fs_in.vTexCoord).rgb;
    for(int i = 0; i < uLightNums; i++)
    {
        vec3 lightDir = normalize(uLightPosArray[i] - fs_in.vFragPos);
        vec3 lightColor = uLightPosColorArray[i];

        //attenuation
        float distance = length(uLightPosArray[i] - fs_in.vFragPos);
        float attenuation = 1.0 / (distance * distance);
        //diffse
        vec3 diffuseColor = diffuseAlbedo * max(dot(lightDir, normal), 0.0) * vec3(1.0) * attenuation;
        resultColor += diffuseColor;
    }
    vec3 ambientColor = vec3(0.1) * diffuseAlbedo;
    
    FragColor = vec4(resultColor, 1.0);
}