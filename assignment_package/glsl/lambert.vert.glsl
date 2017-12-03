#version 150

uniform mat4 u_Model;
uniform mat4 u_ModelInvTr;
uniform mat4 u_ViewProj;
uniform vec4 u_Color;
uniform vec3 u_LookVector;

uniform int u_Time;

in vec4 vs_Pos;
in vec4 vs_Nor;
in vec4 vs_Col;
in vec2 vs_UV;
in vec2 vs_FlowVelocity;
in vec4 vs_Tangent;
in vec4 vs_BiTangent;

in int vs_BlockType;

out vec4 fs_Nor;
out vec4 fs_LightVec;
out vec4 fs_Col;
out vec2 fs_UV;
out vec4 fs_Tangent;
out vec4 fs_BiTangent;
out vec4 hVector;

out float fs_Alpha;

out vec2 flowVelocity;

const vec4 lightDir = normalize(vec4(0.0f, 0.0f, 1.0f, 0.0f));

void main()
{
    int timeWarpFactor = 10;

    float deltaU = ((u_Time * timeWarpFactor) % 625) * 0.0001f;
    float deltaV = ((u_Time * timeWarpFactor) % 625) * 0.0001f;

    fs_Col = vs_Col;                         // Pass the vertex colors to the fragment shader for interpolation
    fs_UV = vec2(deltaU, deltaV) * vs_FlowVelocity + vs_UV;

    mat3 invTranspose = mat3(u_ModelInvTr);
    fs_Nor = vec4(invTranspose * vec3(vs_Nor), 0);

    fs_Tangent = vs_Tangent;
    fs_BiTangent = vs_BiTangent;
    fs_LightVec = lightDir;
    flowVelocity = vs_FlowVelocity;

    if (vs_BlockType == 8)
        fs_Alpha = 0.6f;
    else
        fs_Alpha = 1.0f;

    vec4 modelposition = u_Model * vs_Pos;
    hVector = normalize(normalize(vec4(u_LookVector, 1.0f) - modelposition) + lightDir);

    gl_Position = u_ViewProj * modelposition;
}
