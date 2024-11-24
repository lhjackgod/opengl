#version 420

layout(location = 0) out vec4 FragColor;

in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vFragPos;
uniform sampler2D uTexture;

uniform vec3 uCameraPos;
uniform vec3 uLightPos;

uniform int uBlinnPhong;
void main()
{
    vec3 diffuseColor = vec3(texture(uTexture, vTexCoord));
    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(uLightPos - vFragPos);
    vec3 cameraDir = normalize(uCameraPos - vFragPos);

    //diffuse
    vec3 duffse_light_contribute = max(dot(normal, lightDir), 0.0) * diffuseColor * vec3(1.0);
    //ambient
    vec3 ambient_light_contribute = vec3(0.2) * diffuseColor;
    //specular
    vec3 specular_light_contribute = vec3(0.0);
    if(uBlinnPhong == 1)
    {
        vec3 halfwayDir = normalize(lightDir + cameraDir);
        specular_light_contribute = pow(max(dot(normal, halfwayDir), 0.0), 32.0) * vec3(1.0);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        specular_light_contribute = pow(max(dot(reflectDir, cameraDir), 0.0), 32.0) * vec3(1.0);
    }
    FragColor = vec4(clamp(duffse_light_contribute + ambient_light_contribute + specular_light_contribute, 0.0, 1.0), 1.0);
}