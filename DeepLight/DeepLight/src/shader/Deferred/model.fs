#version 420 core

layout(location = 0) out vec3 ModelPosition;
layout(location = 1) out vec3 ModelNormal;
layout(location = 2) out vec4 ModelAlbedoSpec; 

in VS_OUT
{
    vec3 vFragPos;
    mat3 TBN;
    vec2 vTexCoord;
}fs_in;

struct Textures
{
    sampler2D diffuse0;
    sampler2D normal0;
    sampler2D specular0;
    float ns;
};

uniform Textures uTextures;

void main()
{
    vec3 normal = texture(uTextures.normal0, fs_in.vTexCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(fs_in.TBN * normal);

    vec3 diffuseALBEDO = texture(uTextures.diffuse0, fs_in.vTexCoord).rgb;
    vec3 specularALBEDO = texture(uTextures.specular0, fs_in.vTexCoord).rgb;

    ModelPosition = fs_in.vFragPos;
    ModelNormal = normal;
    ModelAlbedoSpec = vec4(diffuseALBEDO, specularALBEDO.r);
}