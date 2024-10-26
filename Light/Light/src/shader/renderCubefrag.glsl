#version 330 core
out vec4 FragColor;
in vec3 outNormal;
in vec3 outFragPos;
uniform vec3 lightPos; 
uniform vec3 cameraPos;
uniform vec3 ambient;
uniform vec3 lightColor;
uniform vec3 objColor;
uniform int k;
void main() {
	vec3 lightDir = lightPos - outFragPos;
	vec3 cameraDir = cameraPos - outFragPos;
	lightDir = normalize(lightDir);
	cameraDir = normalize(cameraDir);
	vec3 diffuseColor = max(0.0, dot(outNormal, lightDir)) * lightColor * objColor;

	vec3 ambientColor = ambient * lightColor * objColor;
	vec3 h = normalize(lightDir + cameraDir);
	vec3 specularColor = lightColor * objColor * pow(max(0.0, dot(h, outNormal)), k);
	FragColor = vec4((diffuseColor + ambientColor + specularColor), 1.0);
}