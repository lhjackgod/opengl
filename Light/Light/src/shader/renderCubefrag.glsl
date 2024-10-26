#version 330 core
out vec4 FragColor;
in vec3 outNormal;
in vec3 outFragPos;
uniform vec3 cameraPos;
uniform vec3 objColor;
struct PhonMaterial {
	vec3 diffuseMaterial;
	vec3 specularMaterial;
	vec3 ambientMaterial;
	float k;
};
struct Light {
	vec3 position;
	vec3 ambient;
	vec3 specular;
	vec3 diffuse;
};
uniform PhonMaterial phonMaterial;
uniform Light light;
void main() {
	vec3 lightDir = light.position - outFragPos;
	vec3 cameraDir = cameraPos - outFragPos;
	lightDir = normalize(lightDir);
	cameraDir = normalize(cameraDir);
	vec3 diffuseColor = max(0.0, dot(outNormal, lightDir)) * light.diffuse * phonMaterial.diffuseMaterial;

	vec3 ambientColor = light.ambient * phonMaterial.ambientMaterial;
	vec3 h = normalize(lightDir + cameraDir);
	vec3 specularColor = light.specular * phonMaterial.specularMaterial * pow(max(0.0, dot(h, outNormal)), phonMaterial.k);
	FragColor = vec4((diffuseColor + ambientColor + specularColor), 1.0);
}