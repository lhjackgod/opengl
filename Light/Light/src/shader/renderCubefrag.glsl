#version 330 core
#define NUM_LIGHT 4
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
	float k;
};
struct FlashLight {
	vec3 position;
	vec3 diffuse;
	vec3 specular;
	vec3 direction;
	float outCutOff;
	float cutOff;
	float constant;
	float linear;
	float quadtratic;

};
struct pallLight {
	vec3 direction;
	vec3 ambient;
	vec3 specular;
	vec3 diffuse;
};
struct pointLight {
	vec3 position;
	float constant;
	float linear;
	float quadtratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float k;
	vec3 cameraPos;
};
uniform vec3 ambient;
uniform PhonMaterial phonMaterial;
uniform FlashLight flashlight;
uniform Material material;
uniform pallLight pL;
uniform pointLight point_light[NUM_LIGHT];
vec3 calPointLight(pointLight plight, vec3 n, vec3 cameraDir) {
	vec3 lightDir = normalize(plight.position - outFragPos);
	float distance = sqrt(dot(plight.position - outFragPos, plight.position - outFragPos));
	float attenuation = 1.0 / (plight.constant + plight.linear * distance + plight.quadtratic * distance * distance);
	vec3 diffuseColor = max(0.0, dot(n, lightDir)) * plight.diffuse * vec3(texture(material.diffuse, outUV)) * attenuation;
	vec3 h = normalize(lightDir + cameraDir);
	vec3 specular = plight.specular * vec3(texture(material.specular, outUV)) * pow(max(0.0, dot(n, h)), material.k) * attenuation;
	return diffuseColor + specular;
}
vec3 calPallLight(pallLight parallelLight, vec3 n, vec3 cameraDir) {
	vec3 diffuseColor = max(0.0, dot(parallelLight.direction, n)) * parallelLight.diffuse * vec3(texture(material.diffuse, outUV));
	vec3 h = normalize(parallelLight.direction + cameraDir);
	vec3 specular = parallelLight.specular * vec3(texture(material.specular, outUV)) * pow(max(0.0, dot(n, h)), material.k);
	return diffuseColor + specular;
}
vec3 calFlashlight(FlashLight fLight, vec3 n, vec3 cameraDir) {
	vec3 lightDir = normalize(fLight.position - outFragPos);
	float distance = sqrt(dot(fLight.position - outFragPos, fLight.position - outFragPos));
	float attenuation = 1.0 / (fLight.constant + fLight.linear * distance + fLight.quadtratic * distance);
	float cosine = dot(fLight.direction, -lightDir);
	float denominator = fLight.cutOff - fLight.outCutOff;
	float weight = clamp((cosine - fLight.outCutOff) / denominator, 0.0, 1.0);
	vec3 diffuseColor = max(0.0, dot(lightDir, n)) * fLight.diffuse * vec3(texture(material.diffuse, outUV)) * weight * attenuation;
	vec3 h = normalize(lightDir + cameraDir);
	vec3 specular = fLight.specular * vec3(texture(material.specular, outUV)) * pow(max(0.0, dot(n, h)), material.k) * attenuation * weight;
	return diffuseColor + specular;
}
void main() {
	vec3 cameraDir = normalize(cameraPos - outFragPos);
	vec3 normal = normalize(outNormal);
	vec3 parallelResult = calPallLight(pL, normal, cameraDir);
	vec3 ambientColor = vec3(texture(material.diffuse, outUV)) * ambient;
	
	vec3 resultPointColor = vec3(0.0);
	for (int i = 0; i < NUM_LIGHT; i++) {
		resultPointColor += calPointLight(point_light[i], normal, cameraDir);
	}
	vec3 spotLight = calFlashlight(flashlight, normal, cameraDir);
	vec3 resultColor = ambientColor + parallelResult + resultPointColor + spotLight;
	FragColor = vec4(resultColor, 1.0);
}