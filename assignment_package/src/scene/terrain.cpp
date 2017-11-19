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
                this->setBlockAt(x, y, z, EMPTY);
            }
        }
    }
    this->setBlockAt(15, 128, 0, STONE);
    this->setBlockAt(15, 256, 0, GRASS);
    for (std::pair<int64_t, Chunk*> pair : this->ChunkTable)
    {
        pair.second->create();
    }
}
