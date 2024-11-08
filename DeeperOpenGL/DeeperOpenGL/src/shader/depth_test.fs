#version 330 core
layout(location = 0) out vec4 FragColor;

in vec2 v_TexCoord;
uniform vec4 u_Color;
uniform sampler2D u_Texture;
void main(){
    if(u_Color.x > 0.2f){
        FragColor = u_Color;
    }
    else{
        vec4 texColor = texture(u_Texture, v_TexCoord);
        
        FragColor = texColor;
    }
    
}