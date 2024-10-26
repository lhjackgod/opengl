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
	vec3 direction;
	vec3 ambient;
	vec3 specular;
	vec3 diffuse;
	float constant;
	float linear;
	float quadratic;
	float cutoff;
	float outcutoff;
};
struct pallLight {
	vec3 direction;
	vec3 ambient;
	vec3 specular;
	vec3 diffuse;
};
uniform PhonMaterial phonMaterial;
uniform Light light;
uniform Material material;
uniform pallLight pL;
void main() {
	vec3 lightDir = light.position - outFragPos;
	vec3 cameraDir = cameraPos - outFragPos;
	lightDir = normalize(lightDir);
	cameraDir = normalize(cameraDir);
	float cosine = dot(-lightDir, light.direction);
	float epsilon = light.cutoff - light.outcutoff;
	float intensity = clamp((cosine - light.outcutoff) / epsilon, 0.0, 1.0);

	float distance = sqrt(dot(light.position - outFragPos, light.position - outFragPos));
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
	vec3 diffuseColor = max(0.0, dot(outNormal, lightDir)) * light.diffuse * vec3(texture(material.diffuse, outUV)) * attenuation * intensity;

	vec3 ambientColor = vec3(texture(material.diffuse, outUV)) * light.ambient;
	vec3 h = normalize(lightDir + cameraDir);
	vec3 specularColor = light.specular * vec3(texture(material.specular, outUV)) * pow(max(0.0, dot(h, outNormal)), phonMaterial.k) * attenuation * intensity;
	FragColor = vec4((diffuseColor + ambientColor + specularColor), 1.0);

	
}