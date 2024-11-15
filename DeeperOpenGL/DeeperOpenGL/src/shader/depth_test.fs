#version 330 core
layout(location = 0) out vec4 FragColor;
uniform samplerCube u_Texture;
uniform vec3 cameraPos;
in VS_OUT
{
    vec3 v_Normal;
    vec3 v_Pos;
} fs_in;
void main(){
    
    vec3 I = normalize(fs_in.v_Pos - cameraPos);
    vec3 R = reflect(I, normalize(fs_in.v_Normal));
    float ratio = 1.00 / 1.52;
    vec3 Rr = refract(I, normalize(fs_in.v_Normal), ratio);
    vec4 texColor = texture(u_Texture, Rr);
    FragColor = texColor;
}