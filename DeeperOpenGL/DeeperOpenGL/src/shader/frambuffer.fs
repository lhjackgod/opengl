#version 330 core

layout (location = 0) out vec4 FragColor;

uniform sampler2D u_Texture;
in vec2 v_TexCoord;
void main(){
    FragColor = texture(u_Texture, v_TexCoord);
    
}