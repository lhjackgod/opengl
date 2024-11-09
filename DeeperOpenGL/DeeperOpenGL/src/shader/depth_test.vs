#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 v_Normal;
out vec3 v_Pos;
void main(){
    v_Normal = a_Normal;
    vec4 v_Posh = model * vec4(a_Position,1.0);
    v_Pos = vec3(v_Posh / v_Posh.w);
    gl_Position = projection * model * vec4(a_Position,1.0);
}