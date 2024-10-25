#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;
out vec2 uvPos;
uniform mat4 perspect;
uniform mat4 model;
uniform mat4 view;
void main() {
	gl_Position = perspect * view * model* vec4(aPos, 1.0);
	uvPos = aUV;
}