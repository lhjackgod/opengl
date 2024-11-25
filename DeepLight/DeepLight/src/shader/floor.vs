#version 420
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
    vec2 vTexCoord;
    vec3 vNormal;
    vec3 vFragPos;
    vec4 FragPosLightSpace;
} fs_out;
uniform mat4 model;
uniform mat4 LightOrtho;

void main()
{
    fs_out.vTexCoord = aTexCoord;
    fs_out.vNormal = aNormal;
    vec4 vPos = model * vec4(aPos, 1.0);
    fs_out.vFragPos = vec3(vPos / vPos.w);
    fs_out.FragPosLightSpace = LightOrtho * model * vec4(aPos, 1.0);
    gl_Position = perspective * view * model * vec4(aPos, 1.0);
}