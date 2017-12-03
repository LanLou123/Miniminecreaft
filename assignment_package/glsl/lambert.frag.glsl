#version 150

uniform vec4 u_Color;

//uniform vec4 u_LookVector;

uniform sampler2D u_Surface;
uniform sampler2D u_Normal;
uniform sampler2D u_Greyscale;
uniform sampler2D u_GlossPower;
uniform sampler2D u_Duplicate;

in vec4 fs_Nor;
in vec4 fs_LightVec;
in vec4 fs_Col;
in vec2 fs_UV;
in vec4 fs_Tangent;
in vec4 fs_BiTangent;

in vec4 hVector;

out vec4 out_Col;

void main()
{
    mat4 TBN = mat4(normalize(fs_Tangent),
                    normalize(fs_BiTangent),
                    normalize(fs_Nor),
                    vec4(0.0f, 0.0f, 0.0f, 1.0f));
    vec4 normalSample = texture(u_Normal, fs_UV);
    normalSample = normalize(TBN * normalSample);

    vec4 diffuseColor = texture(u_Surface, fs_UV);
    float diffuseIntensity = dot(normalSample, fs_LightVec);
    diffuseIntensity = clamp(diffuseIntensity, 0, 1);
    vec4 diffuseComponent = diffuseIntensity * diffuseColor;

    vec4 ambientColor = diffuseColor;
    float ambientIntensity = 0.2f;
    vec4 ambientComponent = ambientIntensity * ambientColor;

    vec4 specularColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    float specularBase = max(0.0f, dot(hVector, normalSample));
    vec4 powerTexel = texture(u_Surface, fs_UV);
    float specularPower = 0.21f * powerTexel.r + 0.72f * powerTexel.g + 0.07 * powerTexel.b;
    specularPower = specularPower * 128.0f;

    float specularIntensity = max(0.0f, pow(specularBase, specularPower));
    vec4 specularComponent = specularIntensity * specularColor;

    vec4 accumulatedResult = diffuseComponent + ambientComponent + specularComponent;

    out_Col = accumulatedResult;
}
