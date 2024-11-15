#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;
in VS_OUT
{
    vec3 v_Normal;
    vec3 v_Pos;
} gs_in[];

out VS_OUT
{
    vec3 v_Normal;
    vec3 v_Pos;
} gs_out;

vec3 GetNormal()
{
    vec3 d0 = normalize(vec3(gl_in[1].gl_Position / gl_in[1].gl_Position.w - gl_in[0].gl_Position / gl_in[0].gl_Position.w));
    vec3 d1 = normalize(vec3(gl_in[2].gl_Position / gl_in[2].gl_Position.w - gl_in[0].gl_Position / gl_in[0].gl_Position.w));
    return cross(d0, d1);
}
void main()
{
    for(int i=0;i<3;i++){
        gl_Position = gl_in[i].gl_Position + vec4(GetNormal(),0.0);
        gs_out.v_Normal = gs_in[i].v_Normal;
        gs_out.v_Pos = gs_in[i].v_Pos;
        EmitVertex();
    }
    EndPrimitive();
}