#version 330 core

uniform sampler2D u_shadowMap;

in vec4	o_shadowCoord;

layout(location = 0) out vec4 resultingColor;

const float ZNear  = 0.1f;
const float ZFar = 400.f;

float linearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * ZNear * ZFar) / (ZFar + ZNear - z * (ZFar - ZNear));
}

void main(void)
{

    vec3 shadowCoords_dividedbyW = o_shadowCoord.xyz / o_shadowCoord.w;

    vec3 shadowCoords_biased = shadowCoords_dividedbyW.xyz * 0.5f + vec3(0.5f, 0.5f, 0.5f);
    float closestDepth = texture(u_shadowMap, shadowCoords_biased.xy).r;
    float currentDepth = shadowCoords_biased.z;
    float shadow = (currentDepth < closestDepth + 0.001 ) ? 1.f: 0.3f;

    resultingColor.rgb = vec3(shadow);
    resultingColor.a = 1;
}
