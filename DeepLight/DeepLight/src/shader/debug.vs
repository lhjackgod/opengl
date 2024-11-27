#version 420
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(std140, binding = 2) uniform PMatrix
{
    mat4 view;
    mat4 perspective;
};
uniform mat4 model;
out VS_OUT
{
    vec2 vTexCoord;
    vec3 vPos;
} vs_out;
void main()
{
    vs_out.vTexCoord = aTexCoord;
    vec4 pas_pos = model * vec4(aPos, 1.0);
    vs_out.vPos = pas_pos.xyz / pas_pos.w;
    gl_Position = perspective * view * model * vec4(aPos, 1.0);
}