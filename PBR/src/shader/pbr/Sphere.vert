#version 420 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(std140, binding = 2) uniform PMatrix
{
    mat4 view;
    mat4 perspective;
};

uniform mat4 model;
uniform mat3 normalMatrix;

out VS_OUT
{
    vec3 vFragPos;
    vec3 vNormal;
    vec2 vTexCoord;
} vs_out;

void main()
{
    vs_out.vFragPos = (model * vec4(aPos, 1.0) / (model * vec4(aPos, 1.0)).w).xyz;
    vs_out.vNormal = normalize(normalMatrix * aNormal);
    vs_out.vTexCoord = aTexCoord;

    gl_Position = perspective * view * model *vec4(aPos, 1.0);
}