#version 420
layout (location = 0) out vec4 FragColor;

in VS_OUT
{
    vec3 vFragPos;
    vec2 vTexCoord;
    mat3 TBN;
} fs_in;

uniform sampler2D u_DiffuseTexture;
uniform sampler2D u_NormalTex;

uniform vec3 uLightPos;
uniform vec3 uCameraPos;

void main()
{
    vec3 diffuseAlbedo = texture(u_DiffuseTexture, fs_in.vTexCoord).rgb;

    //normal from tangent to world space
    vec3 normal = texture(u_NormalTex, fs_in.vTexCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(fs_in.TBN * normal);

    //lightDir
    vec3 lightDir = normalize(uLightPos - fs_in.vFragPos);

    //cameraDir
    vec3 cameraDir = normalize(uCameraPos - fs_in.vFragPos);

    //halfwayDir
    vec3 halfwayDir = normalize(lightDir + cameraDir);

    //diffuse
    vec3 diffuseColor = diffuseAlbedo * max(dot(normal, lightDir), 0.0) * vec3(1.0);

    //specular
    vec3 specularColor = pow(max(dot(normal, halfwayDir), 0.0), 32.0) * vec3(1.0);

    //ambient
    vec3 ambientColor = diffuseAlbedo * 0.2;

    //resultColor
    vec3 resultColor = diffuseColor + specularColor + ambientColor;

    FragColor = vec4(vec3(resultColor), 1.0);
}