#version 330 core
layout(location = 0) out vec4 FragColor;
const float offset = 1.0f / 300.0f;
in vec2 v_TexCoord;
uniform vec4 u_Color;
uniform sampler2D u_Texture;
void main(){
    if(u_Color.x > 0.2f){
        FragColor = u_Color;
    }
    else{
        vec2 offsets[9] = vec2[](
        vec2(-offset, offset),
        vec2(0.0f, offset),
        vec2(offset, offset),
        vec2(-offset, 0.0f),
        vec2(0.0f, 0.0f),
        vec2(offset, 0.0f),
        vec2(-offset, -offset),
        vec2(0.0f, -offset),
        vec2(offset, -offset)
        );
        float kernel[9] = float[](
            -1.0f, -1.0f, -1.0f,
            -1.0f, 9.0f, -1.0f,
            -1.0f, -1.0f, -1.0f
        );
        vec4 sampleTex;
        for(int i=0; i<9; i++){
            sampleTex += kernel[i] * texture(u_Texture, v_TexCoord + offsets[i]);
        }
        vec4 texColor = texture(u_Texture, v_TexCoord);
        
        float average = (texColor.r * 0.2126 + texColor.g * 0.7152 + texColor.b * 0.0722);
        FragColor = texColor;
    }
    
}