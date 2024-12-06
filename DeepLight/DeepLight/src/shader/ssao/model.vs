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
    mat3 TBN;
    vec3 vFragPos;
    vec2 vTexCoord;
    vec3 Normal;
}vs_out;
uniform mat4 model;
void main()
{
    mat3 tbn_matrix = mat3(transpose(inverse(view * model))); // view space 
    vec3 T = tbn_matrix * aTangent;
    vec3 N = tbn_matrix * aNormal;
    T = T - dot(T, N) * N;
    vec3 B = cross(N, T);
    vs_out.TBN = mat3(T, B, N);
    vs_out.Normal = tbn_matrix * aNormal;
    
    vec4 pasPos = view * model * vec4(aPos, 1.0); // view space
    vs_out.vFragPos = (pasPos / pasPos.w).xyz;
    vs_out.vTexCoord = aTexCoord;
    gl_Position = perspective * view * model * vec4(aPos, 1.0);
}