#version 420 core

layout(location = 0) out vec4 FragColor;

uniform samplerCube cubeMap;

in vec3 vFragPos;

void main()
{
	vec3 color = textureLod(cubeMap, normalize(vFragPos), 4).rgb;

	color /= (vec3(1.0) + color);
	color = pow(color, vec3(1.0 / 2.2));
	FragColor = vec4(color, 1.0);
}