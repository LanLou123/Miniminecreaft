#version 150



uniform mat4 u_ViewProj;    // We're actually passing the inverse of the viewproj
                            // from our CPU, but it's named u_ViewProj so we don't
                            // have to bother rewriting our ShaderProgram class

uniform ivec2 u_Dimensions; // Screen dimensions

uniform vec3 u_Eye; // Camera pos

uniform float u_Time;

out vec4 outColor;

const float PI = 3.14159265359;
const float TWO_PI = 6.28318530718;

// Sunset palette
const vec3 sunset[5] = vec3[](vec3(255, 229, 119) / 255.0,
                               vec3(254, 192, 81) / 255.0,
                               vec3(255, 137, 103) / 255.0,
                               vec3(253, 96, 81) / 255.0,
                               vec3(57, 32, 51) / 255.0);
// Dusk palette
const vec3 dusk[5] = vec3[](vec3(144, 96, 144) / 255.0,
                            vec3(96, 72, 120) / 255.0,
                            vec3(72, 48, 120) / 255.0,
                            vec3(48, 24, 96) / 255.0,
                            vec3(0, 24, 72) / 255.0);
const vec3 sunColor = vec3(255, 255, 190) / 255.0;
const vec3 cloudColor = sunset[3];

vec2 sphereToUV(vec3);
vec3 uvToSunset(vec2);
vec3 uvToDusk(vec2);

float fbm(const in vec2 uv);
float noise(in vec2 uv);
vec2 smoothF(vec2 uv);

void main()
{
    vec2 ndc = (gl_FragCoord.xy / vec2(u_Dimensions)) * 2.0 - 1.0; // -1 to 1 NDC

//    outColor = vec3(ndc * 0.5 + 0.5, 1);

    vec4 p = vec4(ndc.xy, 1, 1); // Pixel at the far clip plane
    p *= 1000.0; // Times far clip plane value
    p = /*Inverse of*/ u_ViewProj * p; // Convert from unhomogenized screen to world

    vec3 rayDir = normalize(p.xyz - u_Eye);

    vec2 uv = sphereToUV(rayDir);

    vec3 skyHue = uvToSunset(uv);

    vec2 uvT1 = uv + vec2(u_Time * 0.001);
    vec2 uvT2 = uv + vec2(u_Time * 0.00005, -u_Time * 0.0002);

    float heightField = fbm(uv + vec2(u_Time * 0.001));

    vec2 slope = vec2(fbm(uvT2 + vec2(1.0/u_Dimensions.x, 0)) - fbm(uvT2 - vec2(1.0/u_Dimensions.x, 0)),
                      fbm(uvT2 + vec2(0, 1.0/u_Dimensions.y)) - fbm(uvT2 - vec2(0, 1.0/u_Dimensions.y)));

    vec3 distortedSkyHue = uvToSunset(uv + slope);

    vec3 distortedDuskHue = uvToDusk(uv + slope);

    vec3 sunDir = normalize(vec3(0, 0.1, 1.0));

    float sunSize = 30; // Sun can exist b/t 0 and 30 degrees from sunDir
    float angle = acos(dot(rayDir, sunDir)) * 360.0 / PI;
    // Draw the sun
    if(angle < sunSize)
    {
        // 0 to 5 degrees is sun itself
        if(angle < 7.5)
        {
            //                                                        VVVVVVVVVVV reduces cloud cover over sun
            outColor = vec4(mix(sunColor, cloudColor, heightField * 0.75 * angle / 30), 1.0f);
        }
        // 7.5 to 30 degrees is sun halo
        else
        {
            vec3 sunBlur = mix(sunColor, distortedSkyHue, (angle - 7.5) / 22.5);
            outColor = vec4(mix(sunBlur, cloudColor, heightField * 0.75 * angle / 30), 1.0f);
        }
    }
    // Draw the sky, blending from sunset to dusk
    else
    {
        float raySunDot = dot(rayDir, sunDir);
        // Sunset, between 1 and 0.75 dot
        if(raySunDot > 0.75)
        {
            outColor = vec4(mix(distortedSkyHue, sunset[3], heightField * 0.75), 1.0f);
        }
        // LERP sunset and dusk, between 0.75 and -0.1 dot
        else if(raySunDot > -0.1)
        {
            float t = (raySunDot - 0.75) / -0.85;
            vec3 skyMix = mix(distortedSkyHue, distortedDuskHue, t);
            outColor = vec4(mix(skyMix, sunset[3], heightField * 0.75), 1.0f);
        }
        // Pure dusk, less than 0.1 dot
        else
        {
            outColor = vec4(mix(distortedDuskHue, sunset[3], heightField * 0.75), 1.0f);
        }
    }
}

vec3 uvToSunset(vec2 uv)
{
    // Below horizon
    if(uv.y < 0.5)
    {
        return sunset[0];
    }
    else if(uv.y < 0.55) // 0.5 to 0.55
    {
        return mix(sunset[0], sunset[1], (uv.y - 0.5) / 0.05);
    }
    else if(uv.y < 0.6)// 0.55 to 0.6
    {
        return mix(sunset[1], sunset[2], (uv.y - 0.55) / 0.05);
    }
    else if(uv.y < 0.65) // 0.6 to 0.65
    {
        return mix(sunset[2], sunset[3], (uv.y - 0.6) / 0.05);
    }
    else if(uv.y < 0.75) // 0.65 to 0.75
    {
        return mix(sunset[3], sunset[4], (uv.y - 0.65) / 0.1);
    }
    return sunset[4]; // 0.75 to 1
}

vec3 uvToDusk(vec2 uv)
{
    // Below horizon
    if(uv.y < 0.5)
    {
        return dusk[0];
    }
    else if(uv.y < 0.55) // 0.5 to 0.55
    {
        return mix(dusk[0], dusk[1], (uv.y - 0.5) / 0.05);
    }
    else if(uv.y < 0.6)// 0.55 to 0.6
    {
        return mix(dusk[1], dusk[2], (uv.y - 0.55) / 0.05);
    }
    else if(uv.y < 0.65) // 0.6 to 0.65
    {
        return mix(dusk[2], dusk[3], (uv.y - 0.6) / 0.05);
    }
    else if(uv.y < 0.75) // 0.65 to 0.75
    {
        return mix(dusk[3], dusk[4], (uv.y - 0.65) / 0.1);
    }
    return dusk[4]; // 0.75 to 1
}

// Convert a point on a sphere to a UV coordinate
vec2 sphereToUV(vec3 p)
{
    float phi = atan(p.z, p.x); // Returns atan(z/x)
    if(phi < 0)
    {
        phi += TWO_PI; // [0, TWO_PI] range now
    }
    // ^^ Could also just add PI to phi, but this shifts where the UV loop from X = 1 to Z = -1.
    float theta = acos(p.y); // [0, PI]
    return vec2(1 - phi / TWO_PI, 1 - theta / PI);
}


vec2 smoothF(vec2 uv)
{
    return uv*uv*(3.-2.*uv);
}

float noise(in vec2 uv)
{
    const float k = 257.;
    vec4 l  = vec4(floor(uv),fract(uv));
    float u = l.x + l.y * k;
    vec4 v  = vec4(u, u+1.,u+k, u+k+1.);
    v       = fract(fract(1.23456789*v)*v/.987654321);
    l.zw    = smoothF(l.zw);
    l.x     = mix(v.x, v.y, l.z);
    l.y     = mix(v.z, v.w, l.z);
    return    mix(l.x, l.y, l.w);
}

float fbm(const in vec2 uv)
{
    float a = 0.5;
    float f = 5.0;
    float n = 0.;
    int it = 8;
    for(int i = 0; i < 32; i++)
    {
        if(i<it)
        {
            n += noise(uv*f)*a;
            a *= .5;
            f *= 2.;
        }
    }
    return n;
}
