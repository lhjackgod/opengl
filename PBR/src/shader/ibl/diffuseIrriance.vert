#version 420 core

layout(location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 perspective;

out vec3 vFragPos;
void main()
{
	vFragPos = aPos;
	vec4 pos = vec4(aPos, 1.0);
	pos = perspective * view * pos;
	gl_Position = pos;
}