#version 330 core
out vec4 FragColor;
uniform vec3 lightColor;
uniform vec3 objColor;
void main() {
	FragColor = vec4(lightColor * objColor, 1.0);
}