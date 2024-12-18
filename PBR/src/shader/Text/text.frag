#version 420 core

layout(location = 0) out vec4 FragColor;

in vec2 vTexCoord;

uniform sampler2D textImage;
uniform vec3 uColor;
void main()
{
	vec4 color = vec4(1.0, 1.0, 1.0, texture(textImage, vTexCoord).r);
	FragColor = color * vec4(uColor,1.0);
}