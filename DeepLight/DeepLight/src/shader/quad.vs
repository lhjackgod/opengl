#version 420
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (std140, binding = 2) uniform PMatrix
{
    mat4 view;
    mat4 perspective;
};

out VS_OUT
{
    vec3 vFragPos;
    vec2 vTexCoord;
    mat3 TBN;
} vs_out;
uniform mat4 model;
void main()
{
    vec3 T = normalize(mat3(model) * tangent);
    vec3 B = normalize(mat3(model) * bitangent);
    vec3 N = normalize(transpose(inverse(mat3(model))) * aNormal);

    vs_out.TBN = mat3(T, B, N);
    vec4 pas_pos = model * vec4(aPos, 1.0);
    vs_out.vFragPos = (pas_pos / pas_pos.w).xyz;
    vs_out.vTexCoord = aTexCoord;
    gl_Position = perspective * view * model * vec4(aPos, 1.0);
}