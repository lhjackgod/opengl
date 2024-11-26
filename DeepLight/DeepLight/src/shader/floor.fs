#version 420

layout(location = 0) out vec4 FragColor;

in VS_OUT
{
    vec2 vTexCoord;
    vec3 vNormal;
    vec3 vFragPos;
} fs_in;

uniform sampler2D uTexture;
uniform samplerCube depthMap;

uniform vec3 uCameraPos;
uniform vec3 uLightPos;

uniform float far_plane;

float is_visable(float bias)
{
    vec3 fragPos_to_light = fs_in.vFragPos - uLightPos;
    
    vec3 dir = normalize(fragPos_to_light);

    float closeDepth = texture(depthMap, dir).r * far_plane;
    float currentDepth = length(fragPos_to_light);
    
    return currentDepth > closeDepth + bias ? 0.0 : 1.0;
}
void main()
{
    vec3 diffuseColor = vec3(texture(uTexture, fs_in.vTexCoord));
    vec3 normal = normalize(fs_in.vNormal);
    vec3 lightDir = normalize(uLightPos - fs_in.vFragPos);
    vec3 cameraDir = normalize(uCameraPos - fs_in.vFragPos);

    float distance = length(uLightPos - fs_in.vFragPos);

    //diffuse
    vec3 duffse_light_contribute = max(dot(normal, lightDir), 0.0) * diffuseColor * vec3(1.0);
    //ambient
    vec3 ambient_light_contribute = vec3(0.2) * diffuseColor;
    //specular
    vec3 specular_light_contribute = vec3(0.0);
    vec3 halfwayDir = normalize(lightDir + cameraDir);
    specular_light_contribute = pow(max(dot(normal, halfwayDir), 0.0), 64.0) * vec3(1.0);

    //calculate bias
    float bias = max(0.05 * (1.0 - dot(lightDir, normal)), 0.005);

    vec3 resultColor = clamp((duffse_light_contribute + specular_light_contribute) * is_visable(bias) +
    ambient_light_contribute
    , 0.0, 1.0);

    //shadow
    //resultColor *= is_visable(fs_in.FragPosLightSpace);
    
    FragColor = vec4(vec3(resultColor), 1.0);
}