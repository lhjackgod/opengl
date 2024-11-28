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
uniform int inverNormal;
out VS_OUT
{
    vec3 vFragPos;
    vec2 vTexCoord;
    vec3 vNormal;
} vs_out;

void main()
{
    vec4 pas_pos = model * vec4(aPos, 1.0);

    vs_out.vFragPos = (pas_pos.xyz / pas_pos.w).xyz;

    vs_out.vTexCoord = aTexCoord;
    vs_out.vNormal = aNormal;
    if(inverNormal == 1)
    {
        vs_out.vNormal = -vs_out.vNormal;
    }
    gl_Position = perspective * view * model * vec4(aPos, 1.0);
}