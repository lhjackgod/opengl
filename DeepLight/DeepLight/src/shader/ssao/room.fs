#version 420 core

layout(location = 0) out vec3 ModelPosition;
layout(location = 1) out vec3 ModelNormal;
layout(location = 2) out vec4 ModelAlbedoSpec;

in VS_OUT
{
    vec3 vNormal;
    vec3 vFragPos;
}fs_in;

uniform vec3 color;

void main()
{
    ModelPosition = fs_in.vFragPos;
    ModelNormal = fs_in.vNormal;
    ModelAlbedoSpec = vec4(vec3(0.95), 1.0);
}