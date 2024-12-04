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

void main()
{
    gl_Position = perspective * view * model * vec4(aPos, 1.0);
}