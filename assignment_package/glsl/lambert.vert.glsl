#version 330 core

uniform mat4 u_Model;
uniform mat4 u_ModelInvTr;
uniform mat4 u_ViewProj;
uniform vec4 u_Color;
uniform vec3 u_LookVector;

uniform int u_Time;

// uniform mat4 u_Model;
// uniform mat4 u_ViewProj;
uniform mat4 u_shadowMat;

//in vec4	vs_Pos;
//in vec4 vs_Col;


in vec4 vs_Pos;
in vec4 vs_Nor;
in vec4 vs_Col;
in vec2 vs_UV;
in vec2 vs_FlowVelocity;
in vec4 vs_Tangent;
in vec4 vs_BiTangent;

in int vs_BlockType;

out vec4 fs_Pos;
out vec4 fs_Nor;
out vec4 fs_LightVec;
out vec4 fs_Col;
out vec2 fs_UV;
out vec4 fs_Tangent;
out vec4 fs_BiTangent;
out vec4 hVector;
out vec4 test;
out vec4 ambientColor;

out float fs_Alpha;

out vec2 flowVelocity;


// Day time ambient tone
const vec3 dayLight = vec3(255, 255, 255) / 255.0f;
// Night time ambient tone
const vec3 nightLight = vec3(0, 4, 20) / 255.0f;
// Sunset average
const vec3 sunsetLight = vec3(253, 96, 81) / 255.0f;

//const vec4 lightDir = normalize(vec4(0.0f, 0.0f, 1.0f, 0.0f));

out vec4 o_shadowCoord;

const vec4 lightDir = normalize(vec4(0.0f, 0.0f, 1.0f, 0.0f));


void main()
{
    //Sun direction changes periodically over time
    float phase = u_Time * 0.001f;
    vec3 sunDir = normalize(vec3(0.0f, sin(phase), cos(phase)));
    vec4 lightDir = vec4(sunDir, 0.0f);

    float reScaledY = 0.5f * (sunDir.y + 1.0f);
    vec3 dayNightColor = mix(nightLight, dayLight, reScaledY);
    ambientColor = vec4(mix(sunsetLight, dayNightColor, abs(sunDir.y)), 1.0f);

    int timeWarpFactor = 10;

    float deltaU = ((u_Time * timeWarpFactor) % 625) * 0.0001f;
    float deltaV = ((u_Time * timeWarpFactor) % 625) * 0.0001f;

    fs_Col = vs_Col*(1/100.0f)*vs_Pos[3];                         // Pass the vertex colors to the fragment shader for interpolation
    fs_UV = vec2(deltaU, deltaV) * vs_FlowVelocity + vs_UV;

    mat3 invTranspose = mat3(u_ModelInvTr);
    fs_Nor = vec4(invTranspose * vec3(vs_Nor), 0);

    fs_Tangent = vs_Tangent;
    fs_BiTangent = vs_BiTangent;
    fs_LightVec = lightDir;
    flowVelocity = vs_FlowVelocity;
    fs_Pos = vs_Pos;
    if (vs_BlockType == 8)
        fs_Alpha = 0.6f;
    else
        fs_Alpha = 1.0f;

    vec4 modelposition = u_Model * vs_Pos;
    hVector = normalize(normalize(vec4(u_LookVector, 1.0f) - modelposition) + lightDir);

    test =  u_ViewProj * modelposition;


    o_shadowCoord = u_shadowMat * modelposition;


    gl_Position = u_ViewProj * modelposition;
}


//uniform mat4 u_Model;
//uniform mat4 u_ViewProj;
//uniform mat4 u_shadowMat;

//in vec4	vs_Pos;
//in vec4 vs_Col;

//out vec4 o_shadowCoord;

//void main(void)
//{
//   vec4 worldPos = u_Model * vs_Pos;

//   o_shadowCoord = u_shadowMat * worldPos;

//   gl_Position	= u_ViewProj * worldPos;
//}
