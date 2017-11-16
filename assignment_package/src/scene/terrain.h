#pragma once
#include <QList>
#include <la.h>
#include "drawable.h"
#include <stdint.h>

// C++ 11 allows us to define the size of an enum. This lets us use only one byte
// of memory to store our different block types. By default, the size of a C++ enum
// is that of an int (so, usually four bytes). This *does* limit us to only 256 different
// block types, but in the scope of this project we'll never get anywhere near that many.
enum BlockType : unsigned char
{
    EMPTY, GRASS, DIRT, STONE
};

class Terrain
{
public:
    Terrain();
    BlockType m_blocks[64][256][64];                    // A 3D list of the blocks in the world.
                                                           // You'll need to replace this with a far more
                                                           // efficient system of storing terrain.
    void CreateTestScene();

    glm::ivec3 dimensions;

    BlockType getBlockAt(int x, int y, int z) const;   // Given a world-space coordinate (which may have negative
                                                           // values) return the block stored at that point in space.
    void setBlockAt(int x, int y, int z, BlockType t); // Given a world-space coordinate (which may have negative
                                                           // values) set the block at that point in space to the
                                                           // given type.
};

class Chunk : public Drawable
{
private:
    BlockType blocks[65536];

    uint64_t xzGlobalPos;
    Chunk* adjPosZ;
    Chunk* adjNegZ;
    Chunk* adjPosX;
    Chunk* adjNegX;

    std::vector<GLfloat> pos;
    std::vector<GLfloat> nor;
    std::vector<GLfloat> col;
    std::vector<GLuint> ele;
public:
    Chunk(OpenGLContext* parent, uint64_t xz);
    BlockType getBlockType(size_t x, size_t y, size_t z) const;
    BlockType& accessBlockType(size_t x, size_t y, size_t z);

    void setAdjPosZ(Chunk* ptr);
    void setAdjNegZ(Chunk* ptr);
    void setAdjPosX(Chunk* ptr);
    void setAdjNegX(Chunk* ptr);

    void fillLeftFace(size_t x, size_t y, size_t z, BlockType type);
    void fillRightFace(size_t x, size_t y, size_t z, BlockType type);
    void fillFrontFace(size_t x, size_t y, size_t z, BlockType type);
    void fillBackFace(size_t x, size_t y, size_t z, BlockType type);
    void fillUpFace(size_t x, size_t y, size_t z, BlockType type);
    void fillDownFace(size_t x, size_t y, size_t z, BlockType type);

    virtual void create() override;
};
