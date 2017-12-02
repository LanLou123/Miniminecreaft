#version 150

uniform mat4 u_Model;
uniform mat4 u_ModelInvTr;
uniform mat4 u_ViewProj;
uniform vec4 u_Color;

in vec4 vs_Pos;

in vec4 vs_Nor;

in vec4 vs_Col;

in vec2 vs_UV;

in int vs_FlowFlag;

out vec4 fs_Nor;
out vec4 fs_LightVec;
out vec4 fs_Col;
out vec2 fs_UV;

const vec4 lightDir = vec4(2,1,1,0);

void main()
{
    fs_Col = vs_Col;                         // Pass the vertex colors to the fragment shader for interpolation
    fs_UV = vs_UV;

    mat3 invTranspose = mat3(u_ModelInvTr);
    fs_Nor = vec4(invTranspose * vec3(vs_Nor), 0);

    vec4 modelposition = u_Model * vs_Pos;

    fs_LightVec = (lightDir);
    gl_Position = u_ViewProj * modelposition;
}
