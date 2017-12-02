#version 150

uniform vec4 u_Color;
uniform int u_Time;

uniform sampler2D u_Surface;
uniform sampler2D u_Normal;
uniform sampler2D u_GreyScale;

in vec4 fs_Nor;
in vec4 fs_LightVec;
in vec4 fs_Col;
in vec2 fs_UV;

out vec4 out_Col;

void main()
{
    vec4 diffuseColor = texture(u_Surface, fs_UV);

    float diffuseTerm = dot(normalize(fs_Nor), normalize(fs_LightVec));
    diffuseTerm = clamp(diffuseTerm, 0, 1);

    float ambientTerm = 0.2;

    float lightIntensity = diffuseTerm + ambientTerm;

    out_Col = vec4(diffuseColor.rgb * lightIntensity, diffuseColor.a);

    //Testing only
    //out_Col = vec4(fs_UV.xy, 0.0f, 1.0f);
}
