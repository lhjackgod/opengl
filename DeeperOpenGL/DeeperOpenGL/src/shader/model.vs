#version 420 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(std140, binding = 0) uniform Matrics
{
    mat4 view;
    mat4 projection;
};
out vec3 vNormal;
out vec2 vTexCoord;
out vec4 vPos;

uniform mat4 model;


void main()
{
    vNormal = vec3(transpose(inverse(model)) * vec4(aNormal, 0.0));
    vPos = projection * view * model * vec4(aPos, 1.0);
    vTexCoord = aTexCoord;
    gl_Position = vPos;
    
}