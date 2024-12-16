#version 420 core

layout(location = 0) out vec4 FragColor;

in vec3 FragPos;

uniform sampler2D cubeMap;

float MY_PI = 3.1415926;
vec2 invAtan = vec2(1.0 / (2.0 * MY_PI), 1.0 / MY_PI);

vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = SampleSphericalMap(normalize(FragPos));
    vec3 color = texture(cubeMap, uv).rgb;

    FragColor = vec4(color, 1.0);
}