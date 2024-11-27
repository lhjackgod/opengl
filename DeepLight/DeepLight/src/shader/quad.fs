#version 420
layout (location = 0) out vec4 FragColor;

in VS_OUT
{
    vec3 vFragPos;
    vec2 vTexCoord;
    mat3 TBN;
    vec3 tangentLightPos;
    vec3 tangentCameraPos;
    vec3 tangentFragPos;
    vec3 vLightPos;
    vec3 vCameraPos;
} fs_in;

uniform sampler2D u_DiffuseTexture;
uniform sampler2D u_NormalTex;
uniform sampler2D u_HeightTex;
uniform float heightScale;

uniform int useHight;

vec2 calculateTexCoord(vec2 texCoord, vec3 viewDir)
{
    const float numLayer = 10.0;
    float layerDepth = 1.0 / numLayer;
    float currentLayerDepth = 0.0;
    vec2 deltaTexCoord = viewDir.xy * heightScale / numLayer;
    float currentDepthMapValue = texture(u_HeightTex, texCoord).r;
    while(currentLayerDepth < currentDepthMapValue)
    {
        currentLayerDepth += layerDepth;
        texCoord -= deltaTexCoord;
        currentDepthMapValue = texture(u_HeightTex, texCoord).r;
    }
    vec2 beforeTexCoord = texCoord + deltaTexCoord;

    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(u_HeightTex, beforeTexCoord).r - currentLayerDepth + layerDepth;

    float weight = afterDepth / (afterDepth - beforeDepth);
    texCoord = weight * beforeTexCoord + (1.0 - weight) * texCoord;

    return texCoord;
}
void main()
{
    //lightDir
    vec3 lightDir = normalize(fs_in.vLightPos - fs_in.vFragPos);

    //cameraDir
    vec3 cameraDir = normalize(fs_in.vCameraPos - fs_in.vFragPos);
    
    //calculate texcoord
    
    vec2 texCoord = fs_in.vTexCoord;

    if(texCoord.x < 0.0 || texCoord.x > 1.0 || texCoord.y < 0.0 || texCoord.y > 1.0)
    {
        discard;
    }
    if(useHight == 1)
    {
        texCoord = calculateTexCoord(texCoord, cameraDir);
    }

    if(texCoord.x < 0.0 || texCoord.x > 1.0 || texCoord.y < 0.0 || texCoord.y > 1.0)
    {
        discard;
    }
    vec3 diffuseAlbedo = texture(u_DiffuseTexture, texCoord).rgb;

    //normal from tangent to world space
    vec3 normal = texture(u_NormalTex, texCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(fs_in.TBN * normal);
    //halfwayDir
    vec3 halfwayDir = normalize(lightDir + cameraDir);

    //diffuse
    vec3 diffuseColor = diffuseAlbedo * max(dot(normal, lightDir), 0.0) * vec3(1.0);

    //specular
    vec3 specularColor = pow(max(dot(normal, halfwayDir), 0.0), 32.0) * vec3(1.0);

    //ambient
    vec3 ambientColor = diffuseAlbedo * 0.5;

    //resultColor
    vec3 resultColor = diffuseColor + specularColor + ambientColor;

    FragColor = vec4(resultColor, 1.0);
}