#version 330 core
layout (location = 0) out vec4 FragColor;

in vec3 vPos;
uniform samplerCube sky;
void main(){
    FragColor = texture(sky, vPos);
}