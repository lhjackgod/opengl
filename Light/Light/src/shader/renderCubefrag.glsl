#version 330 core
out vec4 FragColor;
in vec3 outNormal;
in vec3 outFragPos;
in vec2 outUV;
uniform vec3 cameraPos;
uniform vec3 objColor;
struct PhonMaterial {
	vec3 diffuseMaterial;
	vec3 specularMaterial;
	vec3 ambientMaterial;
	float k;
};
struct Material {
	sampler2D diffuse;
	sampler2D specular;
};
struct Light {
	vec3 position;
	vec3 ambient;
	vec3 specular;
	vec3 diffuse;
};
uniform PhonMaterial phonMaterial;
uniform Light light;
uniform Material material;
void main() {
	vec3 lightDir = light.position - outFragPos;
	vec3 cameraDir = cameraPos - outFragPos;
	lightDir = normalize(lightDir);
	cameraDir = normalize(cameraDir);
	vec3 diffuseColor = max(0.0, dot(outNormal, lightDir)) * light.diffuse * vec3(texture(material.diffuse, outUV));

	vec3 ambientColor = vec3(0.0);
	vec3 h = normalize(lightDir + cameraDir);
	vec3 specularColor = light.specular * vec3(texture(material.specular, outUV)) * pow(max(0.0, dot(h, outNormal)), phonMaterial.k);
	FragColor = vec4((diffuseColor + ambientColor + specularColor), 1.0);
}