#version 420 core

layout(location = 0) out vec4 FragColor;

in vec3 vFragPos;

uniform samplerCube enviromentMap;

float MY_PI = 3.14159265359;
void main()
{
	float sampleCount = 0.0;
	vec3 irradiance = vec3(0.0);
	float sampleDelta = 0.025;
	vec3 dir = normalize(vFragPos);
	vec3 Up = vec3(0.0, 1.0, 0.0);
	vec3 Right = cross(dir, Up);
	Up = cross(Right, dir);


	for(float mu = 0; mu <= MY_PI * 2.0 ; mu += sampleDelta)
	{
		for(float theta = 0; theta <= MY_PI * 0.5; theta += sampleDelta)
		{
			vec3 sampleDir = vec3(0.0);
			sampleDir.x += (sin(theta) * cos(mu));
			sampleDir.y += (cos(theta));
			sampleDir.z += (sin(theta) * sin(mu));

			sampleDir = sampleDir.x * Right + sampleDir.y * Up + sampleDir.z * dir;

			vec3 sampleColor = texture(enviromentMap, normalize(sampleDir)).rgb;
			irradiance += (sampleColor * cos(theta) * sin(theta));

			sampleCount += 1.0;
		}
	}

	irradiance = MY_PI * (irradiance / sampleCount);
	FragColor = vec4(irradiance , 1.0);
}