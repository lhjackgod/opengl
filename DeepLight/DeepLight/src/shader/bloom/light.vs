#version 420 core
layout(location = 0) in vec3 aPos;
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