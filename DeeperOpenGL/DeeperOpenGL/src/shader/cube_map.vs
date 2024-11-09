#version 330 core
layout(location = 0) in vec3 aPos;
uniform mat4 view;
uniform mat4 perspective;
uniform mat4 model;

out vec3 vPos;
void main(){
    vPos = aPos;
    vec4 pos = perspective * view * model * vec4(aPos, 1.0);
    
    gl_Position = pos.xyww;
}