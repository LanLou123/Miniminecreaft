//#include <scene/terrain.h>

//#include <scene/cube.h>

#include <iostream>

//Terrain::Terrain() : dimensions(64, 256, 64)
//{}

//BlockType Terrain::getBlockAt(int x, int y, int z) const
//{
//    // TODO: Make this work with your new block storage!
//    return m_blocks[x][y][z];
//}

//void Terrain::setBlockAt(int x, int y, int z, BlockType t)
//{
//    // TODO: Make this work with your new block storage!
//    m_blocks[x][y][z] = t;
//}

//// Some random random function
//// given vec2 return a value between -1 and 1
//float random (glm::vec2 st)
//{
//    return glm::fract(glm::sin(glm::dot(st, glm::vec2(12.9898,78.233)))* 43758.5453123f);
//}

//// Based on Morgan McGuire @morgan3d
//// https://www.shadertoy.com/view/4dS3Wd
//float noise (glm::vec2 st)
//{
//    glm::vec2 i = glm::floor(st);
//    glm::vec2 f = glm::fract(st);

//    // Four corners in 2D of a tile
//    float a = random(i);
//    float b = random(i + glm::vec2(1.0, 0.0));
//    float c = random(i + glm::vec2(0.0, 1.0));
//    float d = random(i + glm::vec2(1.0, 1.0));

//    glm::vec2 u = f * f * (3.f - 2.f * f);

//    return (1.f - u[0]) * a + u[0] * b + (c - a)* u[1] * (1.f - u[0]) + (d - b) * u[0] * u[1];
//}

//// fBM function
//#define OCTAVES 6
//float fbm (glm::vec2 st)
//{
//    // Initial values
//    float value = 0.0f;
//    float amplitud = 0.5f;

//    // Loop of octaves
//    for (int i = 0; i < OCTAVES; i++) {
//        value += amplitud * noise(st);
//        st *= 2.f;
//        amplitud *= 0.5f;
//    }
//    return value;
//}

//void Terrain::CreateTestScene()
//{
//    // Create the basic terrain floor
//    for(int x = 0; x < 64; ++x)
//    {
//        for(int z = 0; z < 64; ++z)
//        {
//            for(int y = 127; y < 256; ++y)
//            {
//                if(y <= 128)
//                {
//                    if((x + z) % 2 == 0)
//                    {
//                        m_blocks[x][y][z] = STONE;
//                    }
//                    else
//                    {
//                        m_blocks[x][y][z] = DIRT;
//                    }
//                }
//                else
//                {
//                    m_blocks[x][y][z] = EMPTY;
//                }
//            }
//        }
//    }
//    // Add "walls" for collision testing
//    for(int x = 0; x < 64; ++x)
//    {
//        m_blocks[x][129][0] = GRASS;
//        m_blocks[x][130][0] = GRASS;
//        m_blocks[x][129][63] = GRASS;
//        m_blocks[0][130][x] = GRASS;
//    }
//    for(int y = 129; y < 140; ++y)
//    {
//        m_blocks[32][y][32] = GRASS;
//    }
//}

//void Terrain::GenerateTerrainAt(int left, int bottom)
//{
//    for(int x = left; x < left + 64; ++x)
//    {
//        for(int z = bottom; z < bottom + 64; ++z)
//        {
//            float scale = 48.f;
//            glm::vec2 st = glm::vec2(x, z) / scale;
//            float height = 0.2f * fbm(st);

//            int heightInt = (int) (height * 128.f);


//            for(int y = 0; y < 256; ++y)
//            {
//                if(y < 129)
//                {
//                    m_blocks[x][y][z] = EMPTY;
//                }
//                else if(y < 129 + heightInt - 1 && y >= 129)
//                {
//                    m_blocks[x][y][z] = DIRT;
//                }
//                else if(y == 129 + heightInt - 1 && y >= 129)
//                {
//                    m_blocks[x][y][z] = GRASS;
//                }
//                else
//                {
//                    m_blocks[x][y][z] = EMPTY;
//                }

//            }
//        }
//    }
//}


//****************Bowen's************************8//


#include <scene/terrain.h>

#include <scene/cube.h>


Terrain::Terrain() : dimensions(64, 256, 64)
{}

Terrain::~Terrain()

{
    for (std::pair<int64_t, Chunk*> p : this->ChunkTable)
    {
        delete p.second;
    }

}



xzCoords::xzCoords(int64_t x, int64_t z) : x(x), z(z)

{}



int64_t getChunkOrigin(int64_t val)

{

    int64_t valChunk = 0;

    if (val < 0)

    {

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
    int64_t position = Chunk::getXZCoordPacked(xzCoords(x, z));
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



void Terrain::CreateTestScene()
{
    for (unsigned x = 0; x!=32; ++x)
    {
        for (unsigned z = 0; z!=16; ++z)
        {
            for (unsigned y = 0; y!=256; ++y)
            {
                this->setBlockAt(x, y, z, STONE);
            }
        }
    }

    this->setBlockAt(15, 128, 0, STONE);

    this->setBlockAt(16, 128, 0, GRASS);

    for (std::pair<int64_t, Chunk*> pair : this->ChunkTable)
    {
        pair.second->create();
    }

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
    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10 ;j++)
        {
            this->addChunkAt(parent, i * 16, j * 16);
        }
    }

    for(int x = 0; x < 160; ++x)
    {
        std::cout<<x<<std::endl;
        for(int z = 0; z < 160; ++z)
        {
            float scale = 48.f;
            glm::vec2 st = glm::vec2(x, z) / scale;
            float height = 0.2f * fbm(st);

            //std::cout <<" okay" << x <<" " << z<<std::endl;
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
    for (std::pair<int64_t, Chunk*> pair : this->ChunkTable)
    {
        pair.second->create();
    }
    std::cout<<"setup";
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
            float height = 0.2f * fbm(st);

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
    for (std::pair<int64_t, Chunk*> pair : this->ChunkTable)
    {
        pair.second->create();
    }
}
