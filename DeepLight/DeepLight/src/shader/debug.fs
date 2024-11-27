#version 420

layout(location = 0) out vec4 FragColor;

in VS_OUT
{
    vec2 vTexCoord;
    vec3 vPos;
} fs_in;

uniform sampler2D u_DiffuseTex;
uniform sampler2D u_NormalTex;

uniform vec3 uLightPos;
uniform vec3 uCameraPos;
void main()
{
    vec3 diffuseAlbedo = texture(u_DiffuseTex, fs_in.vTexCoord).rgb;
    vec3 normal = texture(u_NormalTex, fs_in.vTexCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    vec3 lightDir = normalize(uLightPos - fs_in.vPos);
    vec3 cameraDir = normalize(uCameraPos - fs_in.vPos);

    vec3 diffuseColor = diffuseAlbedo * max(dot(normal, lightDir), 0.0) * vec3(1.0);
    vec3 halfwayDir = normalize(lightDir + cameraDir);
    vec3 specularColor = pow(max(dot(normal, halfwayDir), 0.0), 64.0) * vec3(1.0);
    vec3 ambientColor = diffuseAlbedo * 0.2;

    vec3 resultColor = clamp(diffuseColor + specularColor +ambientColor, 0.0, 1.0);
    FragColor = vec4(resultColor, 1.0);
}