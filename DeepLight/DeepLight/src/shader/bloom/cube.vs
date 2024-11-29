#version 420 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

layout(std140, binding = 2) uniform PMatrix
{
    mat4 view;
    mat4 perspective;
};

out VS_OUT
{
    vec3 vFragPos;
    vec2 vTexCoord;
    vec3 vNormal;
} vs_out;
uniform mat4 model;

void main()
{
    vs_out.vTexCoord = aTexCoord;
    vs_out.vNormal = aNormal;
    vs_out.vFragPos = ((model * vec4(aPos, 1.0)) / (model * vec4(aPos, 1.0)).w).xyz;
    gl_Position = perspective * view * model * vec4(aPos, 1.0);
}