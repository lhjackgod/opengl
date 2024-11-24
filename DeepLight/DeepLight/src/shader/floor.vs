#version 420
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(std140, binding = 2) uniform PMatrix
{
    mat4 view;
    mat4 perspective;
};

uniform mat4 model;

out vec2 vTexCoord;
out vec3 vNormal;
out vec3 vFragPos;
void main()
{
    vTexCoord = aTexCoord;
    vNormal = aNormal;
    vec4 vPos = model * vec4(aPos, 1.0);
    vFragPos = vec3(vPos / vPos.w);
    gl_Position = perspective * view * model * vec4(aPos, 1.0);
}