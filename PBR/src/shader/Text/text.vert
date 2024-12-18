#version 420 core

layout(location = 0) in vec4 texMessage;

uniform mat4 projection;
out vec2 vTexCoord;
void main()
{
	vec2 aPos = texMessage.xy;
	vec2 aTexCoord = texMessage.zw;
	gl_Position = projection * vec4(aPos, 0.0, 1.0);
	vTexCoord = aTexCoord;
}