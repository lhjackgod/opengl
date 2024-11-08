#version 330 core
layout(location = 0) out vec4 FragColor;

in vec2 v_TexCoord;
uniform sampler2D u_Texture;
void main(){
    FragColor = vec4(vec3(gl_FragCoord.z),1.0);
    //FragColor = texture(u_Texture, v_TexCoord);
}