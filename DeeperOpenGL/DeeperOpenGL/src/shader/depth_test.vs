#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 v_TexCoord;
void main(){
    v_TexCoord = a_TexCoord;
    gl_Position = projection * model * vec4(a_Position,1.0);
}