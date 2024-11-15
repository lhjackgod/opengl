#version 420
layout(triangles)  in;
layout(line_strip, max_vertices = 12) out;
in VS_OUT
{
    vec3 vNormal;
    vec2 vTexCoord;
    vec4 vPos;
} gs_in[];
void main()
{
    for(int i=0;i<3;i++)
    {
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
        gl_Position = gl_in[i].gl_Position + vec4(gs_in[i].vNormal,0.0f) * 0.02f;
        EmitVertex();
        EndPrimitive();
    }

}