
//****************Bowen's************************8//


#include <scene/terrain.h>

#include <scene/cube.h>

#include <iostream>
const int seaLevel=129;
const int river_depth=5;
const int riverbank_width=10;
const float fbm_magnitude=0.2f;
const int step = 3;
Terrain::Terrain() : dimensions(64, 256, 64)
{
      river1=River(10,25,1);
      river2=River(10,210,2);
      create_riverside();
}

Terrain::~Terrain()
{

    for (std::pair<int64_t, Chunk*> p : this->ChunkTable)
    {
        delete p.second;
    }

}

xzCoords::xzCoords(int32_t x, int32_t z) : x(x), z(z)
{}

int64_t getChunkOrigin(int64_t val)
{
    int64_t valChunk = 0;
    if (val < 0)
    {
        ++val;
        valChunk = -val / 16 + 1;
        valChunk *= -16;
    }
    else
    {
        valChunk = val / 16;
        valChunk *= 16;
    }
    return valChunk;
}


BlockType Terrain::getBlockAt(int x, int y, int z) const

{

    // TODO: Make this work with your new block storage!

    int64_t chunkX = getChunkOrigin(x);
    int64_t chunkZ = getChunkOrigin(z);
    Chunk* chunk = this->getChunkAt(chunkX, chunkZ);
    int64_t localX = x - chunkX;
    int64_t localZ = z - chunkZ;
    return chunk->getBlockType(localX, y, localZ);

}



void Terrain::setBlockAt(int x, int y, int z, BlockType t)

{

    // TODO: Make this work with your new block storage!
    int64_t chunkX = getChunkOrigin(x);
    int64_t chunkZ = getChunkOrigin(z);
    Chunk* chunk = this->getChunkAt(chunkX, chunkZ);
    int64_t localX = x - chunkX;
    int64_t localZ = z - chunkZ;
    chunk->accessBlockType(localX, y, localZ) = t;
}

Chunk* Terrain::getChunkAt(int64_t x, int64_t z) const

{
    int64_t chunkX = getChunkOrigin(x);
    int64_t chunkZ = getChunkOrigin(z);
    int64_t position = Chunk::getXZCoordPacked(xzCoords(chunkX, chunkZ));
    if (this->ChunkTable.find(position) != this->ChunkTable.end())
    {
        return this->ChunkTable.at(position);
    }
    else
    {
        return nullptr;
    }
    return nullptr;

}

void Terrain::addChunkAt(OpenGLContext *parent, int x, int z)
{
    int64_t xzCoordinate = Chunk::getXZCoordPacked(xzCoords(x, z));
    Chunk* newChunk = new Chunk(parent, this, xzCoordinate);
    this->ChunkTable[xzCoordinate] = newChunk;

}



//**************MJ's****//
// Some random random function
// given vec2 return a value between -1 and 1
float random (glm::vec2 st)
{
    return glm::fract(glm::sin(glm::dot(st, glm::vec2(12.9898,78.233)))* 43758.5453123f);
}

// Based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
float noise (glm::vec2 st)
{
    glm::vec2 i = glm::floor(st);
    glm::vec2 f = glm::fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + glm::vec2(1.0, 0.0));
    float c = random(i + glm::vec2(0.0, 1.0));
    float d = random(i + glm::vec2(1.0, 1.0));

    glm::vec2 u = f * f * (3.f - 2.f * f);

    return (1.f - u[0]) * a + u[0] * b + (c - a)* u[1] * (1.f - u[0]) + (d - b) * u[0] * u[1];
}

// fBM function
#define OCTAVES 6
float fbm (glm::vec2 st)
{

    // Initial values
    float value = 0.0f;
    float amplitud = 0.5f;

    // Loop of octaves
    for (int i = 0; i < OCTAVES; i++) {
        value += amplitud * noise(st);
        st *= 2.f;
        amplitud *= 0.5f;
    }

    return value;
}

void Terrain::GenerateFirstTerrain(OpenGLContext *parent)
{

//     this->addChunkAt(parent, 0, 0);
//     for(int x = 0; x < 16; x++)
//     {
//         for(int z = 0; z < 16; z++)
//         {

//             for(int y = 0; y < 256; y++)
//             {
//                 this->setBlockAt(x,y,z,EMPTY);
//             }
//         }
//     }
//     this->setBlockAt(0,128,0,GRASS);
   for(int i = 0; i < 4; i++)
   {
       for(int j = 0; j < 4 ;j++)
       {
           this->addChunkAt(parent, i * 16, j * 16);
       }
   }

   for(int x = 0; x < 64; ++x)
   {
       for(int z = 0; z < 64; ++z)
       {
           float scale = 48.f;
           glm::vec2 st = glm::vec2(x, z) / scale;
           float height = fbm_magnitude * fbm(st);
           int heightInt = (int) (height * 128.f);


           for(int y = 0; y < 256; ++y)
           {
               if(y < 129)
               {
                   this->setBlockAt(x,y,z,STONE);
               }
               else if(y < 129 + heightInt - 1 && y >= 129)
               {
                   this->setBlockAt(x,y,z,DIRT);
               }
               else if(y == 129 + heightInt - 1 && y >= 129)
               {
                   this->setBlockAt(x,y,z,GRASS);
               }
               else
               {
                   this->setBlockAt(x,y,z,EMPTY);
               }

           }
       }
   }
    updateFirstRiver();
    for (std::pair<int64_t, Chunk*> pair : this->ChunkTable)
    {
        pair.second->create();
    }

}

void Terrain::GenerateTerrainAt(int left, int bottom,OpenGLContext *parent)
{

    // normalize x and z coord
    int normalX = 0;
    int normalZ = 0;
    if(left >= 0)
    {
        normalX = left / 16;
        normalX *= 16;
    }
    else
    {
        normalX = (- left - 1) / 16 + 1;
        normalX *= -16;
    }
    if(bottom >= 0)
    {
        normalZ = bottom / 16;
        normalZ *= 16;
    }
    else

    {
        normalZ = (- bottom - 1) / 16 + 1;
        normalZ *= -16;
    }
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4 ;j++)
        {
            this->addChunkAt(parent, normalX + i * 16, normalZ + j * 16);
        }
    }
    for(int x = left; x < left + 64; ++x)
    {
        for(int z = bottom; z < bottom + 64; ++z)
        {
            float scale = 48.f;
            glm::vec2 st = glm::vec2(x, z) / scale;
            float height = fbm_magnitude * fbm(st);

            int heightInt = (int) (height * 128.f);

            for(int y = 0; y < 256; ++y)
            {
                if(y < 129)
                {
                    this->setBlockAt(x,y,z,STONE);
                }
                else if(y < 129 + heightInt - 1 && y >= 129)
                {
                    this->setBlockAt(x,y,z,DIRT);
                }
                else if(y == 129 + heightInt - 1 && y >= 129)
                {
                    this->setBlockAt(x,y,z,GRASS);
                }
                else
                {
                    this->setBlockAt(x,y,z,EMPTY);
                }

            }
        }
    }
    int X_max1,X_min1,Z_max1,Z_min1,X_max2,X_min2,Z_max2,Z_min2;
    river1.Get_river_bound(X_min1,X_max1,Z_min1,Z_max1);
    river2.Get_river_bound(X_min2,X_max2,Z_min2,Z_max2);
    if((((X_max1<left)||(Z_max1<bottom))||((X_min1>left+64)||(Z_min1>bottom+64)))&&
        (((X_max2<left)||(Z_max2<bottom))||((X_min2>left+64)||(Z_min2>bottom+64))))
    {}
    else
    {
        updateRiver(left , bottom);
        std::cout<<"river exisit in bound"<<std::endl;
    }
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4 ;j++)
        {
            this->getChunkAt(normalX + i * 16, normalZ + j * 16)->create();
        }
    }

}
//*******************************L-river part implemented by lan lou
void Terrain::update_riverbank()
{

}



bool Terrain::If_Water(int x, int z)
{
    std::tuple<int,int,int> posi=std::make_tuple(x,0,z);
    if(river1.is_river[posi]==true)
        return true;
    else if(river2.is_river[posi]==true)
        return true;
    else
        return false;
}
//this part is meant to create the subtle change in the curvature of the bank so that it looks more real
void Terrain::riverside_curvature(int &height, int &i)
{
    if((i<riverbank_width/3)&&(i>=1))
    height+=step-2;
    if((i<2*riverbank_width/3)&&(i>=riverbank_width/3))
    height+=step-1;
    if((i<=riverbank_width)&&(i>=2*riverbank_width/3))
    height+=step;
}

void Terrain::Calculate_corner_Riverside(int x, int y, int z)//invoked when need to caculate riverside in the corner that have caculation in both x,z direction
{
    int minus_height=y;
    int add_height=y;
    if(If_Water(x,z-1)==false)
    {
        for(int i=1;i<=riverbank_width;i++)
        {

            riverside_curvature(minus_height,i);

            if(If_Water(x,z-i)==false)
            {
                std::tuple<int,int> posside=std::make_tuple(x,z-i);
                if(riverbank_height[posside]>minus_height)
                 riverbank_height[posside]=minus_height;

            }else
                break;
        }
    }
    if(If_Water(x,z+1)==false)
    {
        for(int i=1;i<riverbank_width;i++)
        {
            riverside_curvature(add_height,i);
            if(If_Water(x,z+i)==false)
            {
                std::tuple<int,int> posside=std::make_tuple(x,z+i);
                if(riverbank_height[posside]>add_height)
                 riverbank_height[posside]=add_height;

            }else
                break;
        }
    }
}


void Terrain::create_riverside()
{
    std::cout<<"generating river side................"<<std::endl;
    bool coner = false ;
    int river1_minx = 0; int river2_minx = 0;
    int river1_maxx = 0; int river2_maxx = 0;
    int river1_minz = 0; int river2_minz = 0;
    int river1_maxz = 0; int river2_maxz = 0;
    river1.Get_river_bound(river1_minx,river1_maxx,river1_minz,river1_maxz);
    river2.Get_river_bound(river2_minx,river2_maxx,river2_minz,river2_maxz);
    river1_minx=river1_minx>river2_minx?river2_minx:river1_minx;
    river1_minz=river1_minz>river2_minz?river2_minz:river1_minz;
    river1_maxx=river1_maxx<river2_maxx?river2_maxx:river1_maxx;
    river1_maxz=river1_maxz<river2_maxz?river2_maxz:river1_maxz;
    river1_minx-=riverbank_width;
    river1_minz-=riverbank_width;
    river1_maxx+=riverbank_width;
    river1_maxz+=riverbank_width;
    for( int x = river1_minx ; x <= river1_maxx ; x++)
    {
        for( int z = river1_minz ; z <= river1_maxz ; z++)
        {
            riverbank_height[std::make_tuple(x,z)]=256;
        }
    }

    for( int x = river1_minx ; x < river1_maxx ; x++)
    {
        for( int z = river1_minz ; z < river1_maxz ; z++)
        {
            if(If_Water(x,z))
            {
                int minus_height=0+seaLevel;
                int add_height=0+seaLevel;
                if(If_Water(x,z-1)==false)
                {
                    coner = true;
                    for( int i = 1;i<=riverbank_width;i++)
                    {
                        riverside_curvature(minus_height,i);
                        if(If_Water(x,z-i)==false)
                        {
                        std::tuple<int,int> temp = std::make_tuple(x,z-i);
                        if(riverbank_height[temp]>minus_height)
                            riverbank_height[temp] = minus_height;
                        }else break;
                    }

                }
                if(If_Water(x,z+1)==false)
                {
                    coner = true;
                    for ( int i = 1;i<=riverbank_width;i++)
                    {
                        riverside_curvature(add_height,i);
                        if(If_Water(x,z+i)==false)
                        {
                            std::tuple<int,int> temp= std::make_tuple(x,z+i);
                            if(riverbank_height[temp]>add_height)
                                riverbank_height[temp]=add_height;
                        }else break;

                    }
                }
                int xminus_height = 0+seaLevel;
                int xadd_height = 0+seaLevel;
                if(If_Water(x-1,z)==false)
                {
                for( int i = 0;i<= riverbank_width; i++)
                {
                    riverside_curvature(xminus_height,i);
                    if(If_Water(x-i,z)==false)
                    {
                        std::tuple<int,int> temp= std::make_tuple(x-i,z);
                        if(riverbank_height[temp]>xminus_height)
                        {riverbank_height[temp] = xminus_height;}
                        if(coner)
                            Calculate_corner_Riverside(x-i,xminus_height,z);
                    }
                    else break;
                }

                }
                if(If_Water(x+1,z)==false)
                {
                    for(int i = 1 ;i<riverbank_width;i++)
                    {
                        riverside_curvature(xadd_height,i);
                        if(If_Water(x+i,z)==false)
                        {
                            std::tuple<int,int> temp= std::make_tuple(x+i,z);
                            if(riverbank_height[temp]>xadd_height)
                                riverbank_height[temp] = xadd_height;
                            if(coner)
                                Calculate_corner_Riverside(x+i,xadd_height,z);
                        }else break;
                    }
                }
            }
        }
    }
      std::cout<<"riverside generation completed!!"<<std::endl;
}

void Terrain::updateFirstRiver()//called when first update river in the first default landscape
{
    int max_bound_x = 64;
    int max_bound_z = 64;
    int min_bound_x = 0;
    int min_bound_z = 0;
    int min_bound_y = 0;
    int max_bound_y = 256;
    for( int x = min_bound_x ; x< max_bound_x ; x++)
    {
        for (int z = min_bound_z ; z< max_bound_z ; z++)
        {
            std::tuple<int,int> pos2 = std::make_tuple(x,z);
            std::tuple<int,int,int> pos1 = std::make_tuple(x,0,z);
            if((river1.is_river[pos1]==true)&&
                    this->getBlockAt(x,0,z)!=EMPTY&&this->getBlockAt(x,0,z)!=WATER)
            {
                for(int i = 0 ;i<river_depth ;i++)
                {
                    this->setBlockAt(x,seaLevel-i,z,WATER);
                }
            }
            if((river2.is_river[pos1]==true)&&
                    this->getBlockAt(x,0,z)!=EMPTY&&this->getBlockAt(x,0,z)!=WATER)
            {
                for(int i = 0 ;i<river_depth ;i++)
                {
                    this->setBlockAt(x,seaLevel-i,z,WATER);
                }
            }
            for( int y = min_bound_y ;y<max_bound_y ; y++)
            {
                if((y>0+seaLevel)&&(river1.is_river[pos1]==true))
                {
                    this->setBlockAt(x,y,z,EMPTY);
                }
                if((y>0+seaLevel)&&(river2.is_river[pos1]==true))
                {
                    this->setBlockAt(x,y,z,EMPTY);
                }

            }
            int pos_height = riverbank_height[pos2];
            if((river1.is_river[pos1]==false)&&(river2.is_river[pos1]==false)&&(pos_height<256)&&(pos_height>0))
            {

                if(this->getBlockAt(x,pos_height,z)!=EMPTY)
                {
                    for(int i = riverbank_height[pos2];i<max_bound_y;i++)
                    {
                        this->setBlockAt(x,i,z,EMPTY);
                    }
                    this->setBlockAt(x,riverbank_height[pos2]-1,z,GRASS);
                }
            }

        }
    }
}

void Terrain::updateRiver(int origin_x, int origin_z)//called every time when it is requested to generate new terrain from a certain origin
{
    int max_bound_x = origin_x + 64;
    int max_bound_z = origin_z + 64;
    int min_bound_x = origin_x;
    int min_bound_z = origin_z;
    int min_bound_y = 0;
    int max_bound_y = 256;
    for( int x = min_bound_x ; x< max_bound_x ; x++)
    {
        for (int z = min_bound_z ; z< max_bound_z ; z++)
        {
            std::tuple<int,int,int> pos1 = std::make_tuple(x,0,z);
            std::tuple<int,int> pos2 = std::make_tuple(x,z);
            if((river1.is_river[pos1]==true)&&
                    this->getBlockAt(x,0,z)!=EMPTY&&this->getBlockAt(x,0,z)!=WATER)
            {
                for(int i = 0 ;i<river_depth ;i++)
                {
                    this->setBlockAt(x,seaLevel-i,z,WATER);
                }
            }
            if((river2.is_river[pos1]==true)&&
                    this->getBlockAt(x,0,z)!=EMPTY&&this->getBlockAt(x,0,z)!=WATER)
            {
                for(int i = 0 ;i<river_depth ;i++)
                {
                    this->setBlockAt(x,seaLevel-i,z,WATER);
                }
            }
            for( int y = min_bound_y ;y<max_bound_y ; y++)
            {
                if((y>0+seaLevel)&&(river1.is_river[pos1]==true))
                {
                    this->setBlockAt(x,y,z,EMPTY);
                }
                if((y>0+seaLevel)&&(river2.is_river[pos1]==true))
                {
                    this->setBlockAt(x,y,z,EMPTY);
                }
            }
            int pos_height = riverbank_height[pos2];
            if((river1.is_river[pos1]==false)&&(river2.is_river[pos1]==false)&&(pos_height<256)&&(pos_height>0))
            {

                if(this->getBlockAt(x,pos_height,z)!=EMPTY)
                {
                    for(int i = riverbank_height[pos2];i<max_bound_y;i++)
                    {
                        this->setBlockAt(x,i,z,EMPTY);
                    }
//                    if(this->getBlockAt(x,pos_height-1,z)!=nullptr)
                    this->setBlockAt(x,pos_height-1,z,GRASS);
                }
            }

        }
    }
}
//*******************************************river part end
