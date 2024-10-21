#version 330 core
in vec3 pointColor;
in vec2 uv_position;
out vec4 FragColor;
uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;
void main(){
    FragColor = mix(texture(ourTexture1, uv_position), texture(ourTexture2, uv_position), 0.7);
}