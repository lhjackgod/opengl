#version 420 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out vec3 vColor;
uniform vec2 offset[100];

void main()
{
    vColor = aColor;
    gl_Position = vec4(aPos.xy + offset[gl_InstanceID], 0.0, 1.0);
}