#version 420 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(std140, binding = 0) uniform Matrics
{
    mat4 view;
    mat4 projection;
};
out VS_OUT{
    vec3 vNormal;
    vec2 vTexCoord;
    vec4 vPos;
} vs_out;

uniform mat4 model;


void main()
{
    vs_out.vNormal = vec3(transpose(inverse(model)) * vec4(aNormal, 0.0));
    vs_out.vPos = projection * view * model * vec4(aPos, 1.0);
    vs_out.vTexCoord = aTexCoord;
    gl_Position = vs_out.vPos;
    
}