#version 420
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(std140, binding = 2) uniform PMatrix
{
    mat4 view;
    mat4 perspective;
};
out VS_OUT
{
    vec2 vTexCoord;
    vec3 vNormal;
    vec3 vFragPos;
} fs_out;
uniform mat4 model;
uniform int reverse_normals;
void main()
{
    fs_out.vTexCoord = aTexCoord;
    if(reverse_normals == 1)
    {
        fs_out.vNormal = transpose(inverse(mat3(model))) * (-1.0 * aNormal); 
    }
    else
    {
        fs_out.vNormal = transpose(inverse(mat3(model))) * aNormal;
    }

    vec4 vPos = model * vec4(aPos, 1.0);
    fs_out.vFragPos = vec3(vPos / vPos.w);
    gl_Position = perspective * view * model * vec4(aPos, 1.0);
}