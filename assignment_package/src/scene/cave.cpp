#include "cave.h"
#include <math.h>
#include <iostream>
#define M_PI 3.1415926535
#define OCTAVES 6
#define NUM_NOISE_OCTAVES 5
const float step = 3;

Cave::Cave()
{}
Cave::Cave(int x, int y, int z, float initial_h_degree):
        minX(200),maxX(-200),minZ(200),maxZ(-200),cave_radius(3),cave_length(50),moved_length(0)
{
    Hori_degree = initial_h_degree;
    min_rand = 1;
    max_rand = 0;
    origin_pos_x = x;
    origin_pos_y = y;
    origin_pos_z = z;
    generate_cave();
}
//float Cave::random (glm::vec2 st)
//{
//    return glm::fract(glm::sin(glm::dot(st, glm::vec2(12.9898,78.233)))* 43758.5453123f);
//}

float hash(float n) { return glm::fract(sin(n) * 1e4); }

// Based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
float Cave::noise(glm::vec3 x) {
    const glm::vec3 step = glm::vec3(110, 241, 171);

    glm::vec3 i = glm::floor(x);
    glm::vec3 f = glm::fract(x);

    // For performance, compute the base input to a 1D hash from the integer part of the argument and the
    // incremental change to the 1D based on the 3D -> 1D wrapping
    float n = glm::dot(i, step);

    glm::vec3 u = f * f * (3.0f -f *2.0f  );
    return glm::mix(glm::mix(glm::mix( hash(n + glm::dot(step, glm::vec3(0, 0, 0))), hash(n + glm::dot(step, glm::vec3(1, 0, 0))), u.x),
                   glm::mix( hash(n + glm::dot(step, glm::vec3(0, 1, 0))), hash(n + glm::dot(step, glm::vec3(1, 1, 0))), u.x), u.y),
               glm::mix(glm::mix( hash(n + glm::dot(step, glm::vec3(0, 0, 1))), hash(n + glm::dot(step, glm::vec3(1, 0, 1))), u.x),
                   glm::mix( hash(n + glm::dot(step, glm::vec3(0, 1, 1))), hash(n + glm::dot(step, glm::vec3(1, 1, 1))), u.x), u.y), u.z);
}




float Cave::fbm(glm::vec3 x) {
    float v = 0.0;
    float a = 0.5;
    glm::vec3 shift = glm::vec3(100);
    for (int i = 0; i < NUM_NOISE_OCTAVES; ++i) {
        v += a * noise(x);
        x = x * 2.0f + shift;
        a *= 0.5;
    }
    return v;
}


glm::vec3 Cave::move_cave(glm::vec3 FormerPos)
{

        float fbm_rand = fbm(glm::vec3(FormerPos[0],FormerPos[1],FormerPos[2]));
        float real_fbm = fbm_rand - 0.2;
        if(real_fbm<0) real_fbm = 0;
        real_fbm*=1.2;
        float RandVerticalAngle =/* (rand()%180) + 180*/(real_fbm)*180.0f+180.0f;
        float RandHorizontalAngle = /*(rand()%360)/1.30f*/(real_fbm)*360.0f;
        glm::vec3 newPos;
        newPos[1] = FormerPos[1] + cos(RandVerticalAngle*M_PI/180.0f) * step;
        if(moved_length <=0)
        {
            newPos[0] = FormerPos[0] + cos(Hori_degree*M_PI/180.0f) * 2;
            newPos[2] = FormerPos[2] + sin(Hori_degree*M_PI/180.0f) * 2;
        }
        else
        {
        newPos[0] = FormerPos[0] + cos(RandHorizontalAngle*M_PI/180.0f) * step;
        newPos[2] = FormerPos[2] + sin(RandHorizontalAngle*M_PI/180.0f) * step;
       }

        moved_length++;
        return newPos;
}

void Cave::createLavaPool(glm::vec3 final_pos)
{
    for ( int i = -3*cave_radius ; i<=3*cave_radius ; i++)
    {
        for ( int j = -3*cave_radius; j<=3*cave_radius ; j++)
        {
            for(int k =-3*cave_radius; k<= 3*cave_radius;k++)
            {
            int length1= (i*i+j*j+k*k);
            if(length1<9*cave_radius*cave_radius)
            {
                is_cave[std::make_tuple(final_pos[0]+ i,final_pos[1]+k,final_pos[2]+ j)] = true;
                if(k<-1*cave_radius)
                {
                    is_lavapool[std::make_tuple(final_pos[0]+ i,final_pos[1]+k-2,final_pos[2]+ j)] = true;
                }
            }

            }
        }
    }
}

void Cave::generate_cave()
{

    glm::vec3 current_pos(origin_pos_x , origin_pos_y, origin_pos_z);

    while (moved_length<cave_length)
    {
        glm::vec3 temp  = current_pos;
        for ( int i = -cave_radius ; i<=cave_radius ; i++)
        {
            for ( int j = -cave_radius; j<=cave_radius ; j++)
            {
                for(int k =-cave_radius; k<= cave_radius;k++)
                {
                int length1= (i*i+j*j+k*k);
                if(length1<cave_radius*cave_radius)
                {
                    is_cave[std::make_tuple(current_pos[0]+ i,current_pos[1]+k,current_pos[2]+ j)] = true;
                }
//                is_cave[std::make_tuple(current_pos[0]+ i,current_pos[1]+k,current_pos[2]+ j)] = true;
                }
            }
        }
        float rd_factor=fbm(glm::vec3(current_pos[0],current_pos[1],current_pos[2]));
        current_pos = move_cave(current_pos);
        if(rd_factor>0.6&&rd_factor<0.8)
        {
            for(int i =-1;i<=2;i++)
            {
                for(int j = -1; j<=2;j++)
                {
                    is_ore_type[std::make_tuple(temp[0]+cave_radius+i,temp[1],temp[2]-cave_radius+j)] = 2  ;
                }
            }
        }
        if(rd_factor>0.5&&rd_factor<0.55)
        {
            for(int i =-1;i<=2;i++)
            {
                for(int j = -1; j<=2;j++)
                {
                    is_ore_type[std::make_tuple(temp[0]-cave_radius+i,temp[1],temp[2]+cave_radius+j)] = 1;
                }
            }
        }
        for(int i = temp[1];i>=current_pos[1];i--)
        {
            float ratio = (i-temp[1])/(current_pos[1]-temp[1]);
            float curr_x = temp[0]+ratio*(current_pos[0]-temp[0]);
            float curr_z = temp[2]+ratio*(current_pos[2]-temp[2]);
            for(int j = -cave_radius;j<=cave_radius;j++)
            {
                for(int k = -cave_radius;k<=cave_radius;k++)
                {
                    for(int p = -cave_radius;p<=cave_radius;p++)
                    {
                    int length2 = j*j+k*k+p*p;
                    if(length2<cave_radius*cave_radius)
                    {
                        is_cave[std::make_tuple(curr_x+j,i+p,curr_z+k)]=true;
                    }
//                    is_cave[std::make_tuple(curr_x+j,i+p,curr_z+k)]=true;
                    }
                }
            }
        }
        maxX = maxX<temp[0]?temp[0]:maxX;
        minX = minX>temp[0]?temp[0]:minX;
        maxZ = maxZ<temp[2]?temp[2]:maxX;
        minZ = minZ>temp[2]?temp[2]:minZ;
    }
    createLavaPool(current_pos);
}
