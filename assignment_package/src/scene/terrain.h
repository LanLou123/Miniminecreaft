


//*************Bowen's****************//
#pragma once

#include <QList>

#include <la.h>
#include "drawable.h"
#include <stdint.h>
#include <unordered_map>
#include "drawable.h"

#include <stdint.h>
#include "river.h"
#include <stdint.h>
#include <map>
#include <tuple>
#include "cave.h"
#include <unordered_map>
#include <QRunnable>
#include <QMutex>

// C++ 11 allows us to define the size of an enum. This lets us use only one byte
// of memory to store our different block types. By default, the size of a C++ enum
// is that of an int (so, usually four bytes). This *does* limit us to only 256 different
// block types, but in the scope of this project we'll never get anywhere near that many.

enum BlockType : unsigned char
{

    EMPTY, GRASS, DIRT, STONE, LAVA, WOOD, LEAF, BEDROCK, WATER, ICE, GOLD, IRONORE, COAL, SNOW

};

enum FaceFacing : unsigned char

{
    FRONT, BACK, LEFT, RIGHT, UP, DOWN
};




const BlockType invalidBlockType = (BlockType)0xFF;

class Cave;
class Chunk;
class TerrainAtBoundary;
class Terrain
{
public:

    Terrain( );

    void CreateTestScene();
    glm::ivec3 dimensions;
    std::vector<Cave*> cav_lst;
    River river1;
    River river2;
    Cave* cave1;
    Cave* cave2;
    Cave* cave3;

    std::map<std::tuple<int,int>,int> RiversideHeight;


    BlockType getBlockAt(int x, int y, int z) const;   // Given a world-space coordinate (which may have negative
                                                           // values) return the block stored at that point in space.
    void setBlockAt(int x, int y, int z, BlockType t); // Given a world-space coordinate (which may have negative
                                                       // values) set the block at that point in space to the
                                                           // given type.

    Chunk* getChunkAt(int64_t x, int64_t z) const;


    //void addChunkAt(OpenGLContext* parent, int x, int z);
    Chunk* newChunkAt(OpenGLContext* parent, int x, int z);
    void addChunk2Map(Chunk* chunk);

    std::unordered_map<int64_t, Chunk*> ChunkTable;
    // Generate a 64* 256 * 64 Terrain at a given point(the bottom-left of new terrain)
    void GenerateTerrainAt(int left, int bottom, OpenGLContext *parent);
    void GenerateFirstTerrain(OpenGLContext *parent);

//*********************end
//*********************lanlou L river part
    std::map<std::tuple<int,int>,int> riverbank_height;
    bool If_Water(int x,int y);
    void update_riverbank();
    void updateFirstRiver();
    void riverside_curvature(int &height,  int &i);
    void updateRiver(int origin_x, int origin_z, Chunk* locatedChunk);
    void Calculate_corner_Riverside(int x, int y, int z);
    void create_riverside();
//**********************end
    void updateCave();
    ~Terrain();

    // friend class TerrainAtBoundary;

private:
    QMutex* chunkMutex;

};



struct xzCoords

{
    int32_t x;
    int32_t z;
    xzCoords(int32_t x, int32_t z);
};


class Chunk : public Drawable
{

friend class Terrain;

private:

    BlockType blocks[65536];
    int heightField[256];
    int64_t xzGlobalPos;
    Terrain* terrain;

    static std::vector<GLfloat> pos;
    static std::vector<GLfloat> nor;
    static std::vector<GLfloat> uv;
    static std::vector<GLfloat> flowVelocity;
    static std::vector<GLuint> ele;
    static std::vector<GLfloat> tan;
    static std::vector<GLfloat> bitan;
    static std::vector<GLint> buftype;

    static std::vector<GLfloat> posF;
    static std::vector<GLfloat> norF;
    static std::vector<GLfloat> uvF;
    static std::vector<GLfloat> flowVelocityF;
    static std::vector<GLuint> eleF;
    static std::vector<GLfloat> tanF;
    static std::vector<GLfloat> bitanF;
    static std::vector<GLint> buftypeF;

    int& accessHeightAt(size_t x, size_t z);

    Chunk* getLeftAdjacent();
    Chunk* getRightAdjacent();
    Chunk* getBackAdjacent();
    Chunk* getFrontAdjacent();

    void fillLeftFace(size_t x, size_t y, size_t z, BlockType type);
    void fillRightFace(size_t x, size_t y, size_t z, BlockType type);
    void fillFrontFace(size_t x, size_t y, size_t z, BlockType type);
    void fillBackFace(size_t x, size_t y, size_t z, BlockType type);
    void fillUpFace(size_t x, size_t y, size_t z, BlockType type);
    void fillDownFace(size_t x, size_t y, size_t z, BlockType type);


    void fillFace(glm::vec4 positions[], glm::vec4 normal, BlockType type, FaceFacing facing);

    void appendUV(std::vector<GLfloat>* container, const glm::vec2 uvCoords[]);

    void appendFlow(std::vector<GLfloat>* container, glm::vec2 speed);

    BlockType getBlockType(size_t x, size_t y, size_t z);

    BlockType& accessBlockType(size_t x, size_t y, size_t z);

    bool shouldFill(size_t x, size_t y, size_t z, BlockType currentBlock);

public:

    Chunk(OpenGLContext* parent, Terrain *terrain, int64_t xz);

    int& accessHeightAtGlobal(int x, int z);

    int64_t getXZGlobalPositions();

    static xzCoords getXZCoordUnpacked(int64_t p)
    {
        int32_t x = p >> 32;
        int32_t z = p & 0x00000000FFFFFFFF;

        return xzCoords(x, z);
    }


    BlockType& accessBlockTypeGlobalCoords(int x, int y, int z);

    static int64_t getXZCoordPacked(xzCoords c)
    {
        int64_t xExtended = c.x;
        int64_t zExtended = c.z;
        return xExtended << 32 | (zExtended & 0x00000000FFFFFFFF);
    }
    virtual void create() override;

};


class TerrainAtBoundary : public QRunnable
{
private:

    QMutex* chunkMutex;
    QMutex* checkingMutex;
    std::vector<Chunk*> *chunkToAdd;
    OpenGLContext *parent;
    Terrain* currentTerrain;
public:
    int left;
    int bottom;

    TerrainAtBoundary(int cornerX,
                      int cornerZ,
                      QMutex* m,
                      QMutex* m1,
                      std::vector<Chunk*> *chunkToAdd,
                      Terrain* currentTerrain,
                      OpenGLContext *parent);
    void setLeftBottom(int newLeft, int newBottom);

    void run() override;
};
