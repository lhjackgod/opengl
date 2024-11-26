#version 420

out vec4 FragColor;
in vec4 FragPos;

uniform vec3 LightPos;
uniform float far_plane;
void main()
{
    vec3 fragPos = (FragPos / FragPos.w).xyz;
    float depth = clamp(length(fragPos - LightPos) / far_plane, 0.0, 1.0);
    gl_FragDepth = depth;
    FragColor = vec4(vec3(depth), 1.0);
}