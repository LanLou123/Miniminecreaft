#version 330


uniform mat4 u_ShadowViewProjMat;


uniform mat4 u_Model;
uniform mat4 u_ViewProj;

in vec4 vs_Pos;
//in vec4 vs_Col;

//out vec4 fs_Col;

void main()
{
//    fs_Col = vs_Col;
    gl_Position = u_ShadowViewProjMat* u_Model * vs_Pos;
}
