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
const vec3 sunset[5] = vec3[](vec3(255, 229, 119) / 255.0f,
                               vec3(254, 192, 81) / 255.0f,
                               vec3(255, 137, 103) / 255.0f,
                               vec3(253, 96, 81) / 255.0f,
                               vec3(57, 32, 51) / 255.0f);
// Dusk palette
const vec3 dusk[5] = vec3[](vec3(144, 96, 144) / 255.0f,
                            vec3(96, 72, 120) / 255.0f,
                            vec3(72, 48, 120) / 255.0f,
                            vec3(48, 24, 96) / 255.0f,
                            vec3(0, 24, 72) / 255.0f);

// Day time sky tone
const vec3 dayLight = vec3(0, 165, 255) / 255.0f;
// Night time sky tone
const vec3 nightLight = vec3(0, 4, 20) / 255.0f;
// Day time cloud
const vec3 dayTimeCloud = vec3(255, 255, 255) / 255.0f;
// Night time cloud
const vec3 nightTimeCloud = vec3(64, 64, 64) / 255.0f;

const vec3 sunColor = vec3(255, 255, 190) / 255.0;
const vec3 cloudColor = sunset[3];

vec2 sphereToUV(vec3);
vec3 uvToSunset(float y);
vec3 uvToDusk(float y);

float fbm(const in vec2 uv);
float noise(in vec2 uv);
vec2 smoothF(vec2 uv);

void main()
{
    //Fragment coords 2 NDC coords
    vec2 ndc = (gl_FragCoord.xy / vec2(u_Dimensions)) * 2.0f - 1.0f;

    //Project the fragment to the far clip plane
    vec4 p = vec4(ndc.xy, 1.0f, 1.0f);
    p *= 1000.0f;
    //Fragment position in the world
    p = /*Inverse of*/ u_ViewProj * p;

    //Vector from Camera position 2 fragment position
    vec3 rayDir = normalize(p.xyz - u_Eye);
    //Sun direction changes periodically over time
    float phase = u_Time * 0.001f;
    vec3 sunDir = normalize(vec3(0.0f, sin(phase), cos(phase)));

    //Map this vector to the uv using spherical coordinates
    vec2 uv = sphereToUV(rayDir);

    //Generate heightfield using fbm noise
    vec2 uvT1 = uv + vec2(u_Time * 0.00005, -u_Time * 0.0002f);
    float heightField = fbm(uv + vec2(u_Time * 0.001f));
    //Calculate the derivative as an offset to the uv
    vec2 slope = vec2(fbm(uvT1 + vec2(1.0f / u_Dimensions.x, 0.0f)) - fbm(uvT1 - vec2(1.0f / u_Dimensions.x, 0.0f)),
                      fbm(uvT1 + vec2(0.0f, 1.0f / u_Dimensions.y)) - fbm(uvT1 - vec2(0.0f, 1.0f / u_Dimensions.y)));

    //Sample from sunset color using this uv
    vec3 distortedSunsetHue = uvToSunset((uv + slope).y);

    //Sample from dusk color using this uv
    vec3 distortedDuskHue = uvToDusk((uv + slope).y);

    //Deriving the angle between the sun and the ray.
    float raySunDot = dot(rayDir, sunDir);
    //Cone size of the sun.
    float sunSize = 30.0f;
    float angle = acos(raySunDot) * 360.0f / PI;

    //Pre-render how the sky should be when it's sunset/dawn/day/night
    vec3 sunsetColor = vec3(0.0f);

    //Ray direction's very close to sun
    if (raySunDot > 0.75f)
    {
        sunsetColor = vec3(mix(distortedSunsetHue, sunset[3], heightField * 0.75f));
    }
    // LERP sunset and dusk, between 0.75 and -0.1 dot
    else if (raySunDot > -0.1f)
    {
        float t = (raySunDot - 0.75f) / -0.85f;
        vec3 skyMix = mix(distortedSunsetHue, distortedDuskHue, t);
        sunsetColor = vec3(mix(skyMix, sunset[3], heightField * 0.75f));
    }
    // Pure dusk, less than 0.1 dot
    else
    {
        sunsetColor = vec3(mix(distortedDuskHue, sunset[3], heightField * 0.75f));
    }

    float reScaledY = 0.5f * (sunDir.y + 1.0f);
    vec3 dayTimeColor = mix(dayLight, dayTimeCloud, heightField * 0.75f);
    vec3 nightTimeColor = mix(nightLight, nightTimeCloud, heightField * 0.75f);

    vec3 dayNightColor = mix(nightTimeColor, dayTimeColor, reScaledY);
    vec3 skyColor = mix(sunsetColor, dayNightColor, abs(sunDir.y));

    // Draw the sun
    if (angle < sunSize)
    {
        //Sun itself
        if(angle < 7.5f)
        {
            //Blend the sun color with the cloud
            outColor = vec4(mix(sunColor, cloudColor, heightField * 0.75f * angle / 30.0f), 1.0f);
        }
        //Halo of the sun
        else
        {
            float blendFactor = (angle - 7.5f) / 22.5f;
            vec3 sunBlur = mix(sunColor, skyColor, blendFactor);
            outColor = vec4(sunBlur, 1.0f);
        }
    }
    // Draw the sky using previously derived sky color
    else
    {
        outColor = vec4(skyColor, 1.0f);
    }
}

vec3 uvToSunset(float y)
{
    y = abs(y);
    // Below horizon
    if(y < 0.5f)
    {
        return sunset[0];
    }
    else if(y < 0.55f) // 0.5 to 0.55
    {
        return mix(sunset[0], sunset[1], (y - 0.5) / 0.05f);
    }
    else if(y < 0.6f)// 0.55 to 0.6
    {
        return mix(sunset[1], sunset[2], (y - 0.55f) / 0.05f);
    }
    else if(y < 0.65f) // 0.6 to 0.65
    {
        return mix(sunset[2], sunset[3], (y - 0.6f) / 0.05f);
    }
    else if(y < 0.75f) // 0.65 to 0.75
    {
        return mix(sunset[3], sunset[4], (y - 0.65f) / 0.1f);
    }
    return sunset[4]; // 0.75 to 1
}

vec3 uvToDusk(float y)
{
    y = abs(y);
    // Below horizon
    if(y < 0.5f)
    {
        return dusk[0];
    }
    else if(y < 0.55f) // 0.5 to 0.55
    {
        return mix(dusk[0], dusk[1], (y - 0.5) / 0.05f);
    }
    else if(y < 0.6f)// 0.55 to 0.6
    {
        return mix(dusk[1], dusk[2], (y - 0.55f) / 0.05f);
    }
    else if(y < 0.65f) // 0.6 to 0.65
    {
        return mix(dusk[2], dusk[3], (y - 0.6f) / 0.05f);
    }
    else if(y < 0.75f) // 0.65 to 0.75
    {
        return mix(dusk[3], dusk[4], (y - 0.65f) / 0.1f);
    }
    return dusk[4]; // 0.75 to 1
}

//Map a point in the spherical coords to a uv value
vec2 sphereToUV(vec3 p)
{
    //Fixed the singularity problem.
    float projectXZ = sqrt(p.x*p.x + p.z*p.z);
    float phi = acos(p.x / projectXZ);
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
