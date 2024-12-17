#version 420 core
layout(location = 0) out vec4 FragColor;

in vec2 vTexCoord;
uniform sampler screenTex;

void main()
{
	FragColor = vec4(texture(screenTex, vTexCoord).rg, 1.0);
}