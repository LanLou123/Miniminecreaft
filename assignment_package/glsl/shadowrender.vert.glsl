#version 330 core

uniform mat4 u_Model;
uniform mat4 u_ViewProj;
uniform mat4 u_shadowMat;

in vec4	vs_Pos;
in vec4 vs_Col;

out vec4 o_shadowCoord;

void main(void)
{
   vec4 worldPos = u_Model * vs_Pos;

   o_shadowCoord = u_shadowMat * worldPos;

   gl_Position	= u_ViewProj * worldPos;
}
