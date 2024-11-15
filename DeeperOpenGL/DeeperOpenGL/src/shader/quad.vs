#version 420 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aOffset;
out vec3 vColor;

void main()
{
    vColor = aColor;
    vec3 pos = aPos * (gl_InstanceID / 100.0);
    gl_Position = vec4(pos.xy + aOffset, 0.0, 1.0);
}