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
    vec3 tangentLightPos;
    vec3 tangentCameraPos;
} vs_out;
uniform mat4 model;

uniform vec3 uLightPos;
uniform vec3 uCameraPos;

void main()
{
    vec3 T = normalize(transpose(inverse(mat3(model))) * tangent);
    vec3 N = normalize(transpose(inverse(mat3(model))) * aNormal);
    T = normalize(T - dot(T,N) * N);

    vec3 B = cross(N, T);
    vs_out.TBN = mat3(T, B, N);
    vs_out.TBN = inverse(vs_out.TBN);

    vs_out.tangentLightPos = vs_out.TBN * uLightPos;
    vs_out.tangentCameraPos = vs_out.TBN * uCameraPos;

    vec4 pas_pos = model * vec4(aPos, 1.0);
    vs_out.vFragPos = vs_out.TBN * (pas_pos / pas_pos.w).xyz;

    vs_out.vTexCoord = aTexCoord;
    gl_Position = perspective * view * model * vec4(aPos, 1.0);
}