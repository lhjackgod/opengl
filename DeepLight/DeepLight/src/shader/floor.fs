#version 420

layout(location = 0) out vec4 FragColor;

in VS_OUT
{
    vec2 vTexCoord;
    vec3 vNormal;
    vec3 vFragPos;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D uTexture;
uniform sampler2D depthMap;

uniform vec3 uCameraPos;
uniform vec3 uLightPos;

float is_visable(vec4 fragPosLightSpace)
{
    vec3 frag_pos_light_space = fragPosLightSpace.xyz / fragPosLightSpace.w;
    frag_pos_light_space = frag_pos_light_space * 0.5 + 0.5;
    float closeDepth = texture(depthMap, frag_pos_light_space.xy).r;

    float visibility = frag_pos_light_space.z > closeDepth ? 0.0 : 1.0;
    return visibility;
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
    vec3 resultColor = clamp((duffse_light_contribute + specular_light_contribute) * is_visable(fs_in.FragPosLightSpace) +
    ambient_light_contribute
    , 0.0, 1.0);

    //shadow
    //resultColor *= is_visable(fs_in.FragPosLightSpace);

    FragColor = vec4(resultColor, 1.0);
}