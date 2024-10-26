#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;
out vec3 outNormal;
out vec3 outFragPos;
out vec2 outUV;
void main() {
	gl_Position =perspective * view * model * vec4(aPos, 1.0);
	outNormal = normalize(vec3(transpose(inverse(model)) * vec4(aNormal, 0.0)));
	vec4 homoPos = model * vec4(aPos, 1.0);
	homoPos /= homoPos.w;
	outFragPos = vec3(homoPos);
	outUV = aUV;
}