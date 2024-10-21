#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aUV;
out vec3 pointColor;
out vec2 uv_position;
void main(){
    gl_Position = vec4(aPos, 1.0);
    pointColor = aColor;
    uv_position = aUV;
}