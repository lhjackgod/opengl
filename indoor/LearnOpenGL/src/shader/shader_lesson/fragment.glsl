#version 330 core
in vec4 vertexColor;
uniform vec4 outColor;
out vec4 FragColor;
void main(){
    FragColor = vertexColor;
}