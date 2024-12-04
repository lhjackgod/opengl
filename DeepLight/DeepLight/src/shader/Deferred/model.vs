#version 420 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aTangent;
layout(location = 3) in vec2 aTexCoord;
layout(std140, binding = 2) uniform PMatrix
{
    mat4 view;
    mat4 perspective;
};

out VS_OUT
{
    vec3 vFragPos;
    mat3 TBN;
    vec2 vTexCoord;
}vs_out;

uniform mat4 model;
void main()
{
    mat3 TBN_M = mat3(transpose(inverse(model)));
    vec3 T = normalize(TBN_M * aTangent);
    vec3 N = normalize(TBN_M * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = normalize(cross(N, T));
    vs_out.TBN = mat3(T, B, N);

    vec4 pasPos = model * vec4(aPos, 1.0);
    vs_out.vFragPos = (pasPos / pasPos.w).xyz;
    vs_out.vTexCoord = aTexCoord;
    gl_Position = perspective * view * model * vec4(aPos, 1.0);
}