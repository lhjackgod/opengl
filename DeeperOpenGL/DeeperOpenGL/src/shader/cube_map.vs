#version 420 core
layout(location = 0) in vec3 aPos;
layout(std140, binding = 0) uniform Matrics
{
    mat4 view;
    mat4 perspective;
};
uniform mat4 model;

out vec3 vPos;
void main(){
    vPos = aPos;
    mat4 skyView = mat4(mat3(view));
    //skyView[3] = vec4(0.0,0.0,0.0,1.0);
    vec4 pos = perspective * skyView * model * vec4(aPos, 1.0);
    
    gl_Position = pos.xyww;
}