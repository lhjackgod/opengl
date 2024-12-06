#version 420 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(std140, binding = 2) uniform PMatrix
{
    mat4 view;
    mat4 perspective;
};
uniform bool inverseNormal;
uniform mat4 model;

out VS_OUT
{
    vec3 vNormal;
    vec3 vFragPos;
}vs_out;

void main()
{
    vs_out.vFragPos = ((view * model * vec4(aPos, 1.0)) / (view * model * vec4(aPos, 1.0)).w).xyz; // view space
    
    vec3 pasNormal = mat3(transpose(inverse(view * model))) * (inverseNormal ? -aNormal : aNormal); // view space
    vs_out.vNormal = normalize(pasNormal);
    gl_Position = perspective * view * model * vec4(aPos, 1.0);
}