#version 420 core

layout(location = 0) in vec3 aPos;

out vec3 FragPos;

uniform mat4 perspective;
uniform mat4 view;

void main()
{
    FragPos = aPos;
    gl_Position = perspective * view * vec4(aPos, 1.0);
}