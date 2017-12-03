#version 150

uniform mat4 u_Model;
uniform mat4 u_ModelInvTr;
uniform mat4 u_ViewProj;
uniform vec4 u_Color;

uniform int u_Time;

in vec4 vs_Pos;
in vec4 vs_Nor;
in vec4 vs_Col;
in vec2 vs_UV;
in vec2 vs_FlowVelocity;

out vec4 fs_Nor;
out vec4 fs_LightVec;
out vec4 fs_Col;
out vec2 fs_UV;

const vec4 lightDir = vec4(2,1,1,0);

void main()
{
    float deltaU = (u_Time % 625) * 0.0001f;
    float deltaV = (u_Time % 625) * 0.0001f;

    fs_Col = vs_Col;                         // Pass the vertex colors to the fragment shader for interpolation
    fs_UV = vec2(deltaU, deltaV) * vs_FlowVelocity + vs_UV;

    mat3 invTranspose = mat3(u_ModelInvTr);
    fs_Nor = vec4(invTranspose * vec3(vs_Nor), 0);

    vec4 modelposition = u_Model * vs_Pos;

    fs_LightVec = (lightDir);
    gl_Position = u_ViewProj * modelposition;
}
