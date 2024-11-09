#version 330 core
layout(location = 0) in vec3 aPos;
uniform mat4 viewPerspective;
uniform mat4 model;

out vec3 vPos;
void main(){
    vPos = aPos;
    gl_Position = viewPerspective * model * vec4(aPos, 1.0);
}