#version 330 core
layout(location = 0) out vec4 FragColor;
uniform samplerCube u_Texture;
uniform vec3 cameraPos;
in vec3 v_Normal;
in vec3 v_Pos;
void main(){
    
    vec3 I = normalize(v_Pos - cameraPos);
    vec3 R = reflect(I, normalize(v_Normal));
    float ratio = 1.00 / 1.52;
    vec3 Rr = refract(I, normalize(v_Normal), ratio);
    vec4 texColor = texture(u_Texture, Rr);
    FragColor = texColor;
}