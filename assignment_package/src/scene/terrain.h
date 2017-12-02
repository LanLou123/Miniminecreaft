


//*************Bowen's****************//
#pragma once

#include <QList>

#include <la.h>
#include "drawable.h"
#include <stdint.h>
#include <unordered_map>

#include "drawable.h"
#include "river.h"
#include <stdint.h>
#include <map>
#include <tuple>
#include <unordered_map>



// C++ 11 allows us to define the size of an enum. This lets us use only one byte

// of memory to store our different block types. By default, the size of a C++ enum

// is that of an int (so, usually four bytes). This *does* limit us to only 256 different

// block types, but in the scope of this project we'll never get anywhere near that many.

enum BlockType : unsigned char

{

    EMPTY, GRASS, DIRT, STONE, LAVA

};


class Chunk;


class Terrain

{

public:

    Terrain();

    void CreateTestScene();



    glm::ivec3 dimensions;
    River river1;
    River river2;
    std::map<std::tuple<int,int>,int> RiversideHeight;

    BlockType getBlockAt(int x, int y, int z) const;   // Given a world-space coordinate (which may have negative

                                                           // values) return the block stored at that point in space.

    void setBlockAt(int x, int y, int z, BlockType t); // Given a world-space coordinate (which may have negative

                                                           // values) set the block at that point in space to the

                                                           // given type.


    Chunk* getChunkAt(int64_t x, int64_t z) const;

    void addChunkAt(OpenGLContext* parent, int x, int z);



    std::unordered_map<int64_t, Chunk*> ChunkTable;

//***********mj's
    void GenerateTerrainAt(int left, int bottom, OpenGLContext *parent);        // Generate a 64* 256 * 64 Terrain at a given point(the bottom-left of new terrain)

    void GenerateFirstTerrain(OpenGLContext *parent);
//*********************end
//*********************lanlou L river part
    std::map<std::tuple<int,int>,int> riverbank_height;
    bool If_Water(int x,int y);
    void update_riverbank();
    void updateFirstRiver();
    void riverside_curvature(int &height,  int &i);
    void updateRiver(int origin_x, int origin_z);
    void Calculate_corner_Riverside(int x, int y, int z);
    void create_riverside();
//**********************end
    ~Terrain();

};



struct xzCoords

{

    int32_t x;

    int32_t z;

    xzCoords(int32_t x, int32_t z);

};



class Chunk : public Drawable

{

private:

    BlockType blocks[65536];



    int64_t xzGlobalPos;

    Terrain* terrain;



    std::vector<GLfloat> pos;

    std::vector<GLfloat> nor;

    std::vector<GLfloat> col;

    std::vector<GLuint> ele;



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



    void fillFace(glm::vec4 positions[], glm::vec4 normal, BlockType type);



public:

    Chunk(OpenGLContext* parent, Terrain *terrain, int64_t xz);

    BlockType getBlockType(size_t x, size_t y, size_t z) const;

    BlockType& accessBlockType(size_t x, size_t y, size_t z);



    static xzCoords getXZCoordUnpacked(int64_t p)

    {
        int32_t x = p >> 32;
        int32_t z = p & 0x00000000FFFFFFFF;

        return xzCoords(x, z);

    }



    static int64_t getXZCoordPacked(xzCoords c)

    {
        int64_t xExtended = c.x;
        int64_t zExtended = c.z;
        return xExtended << 32 | (zExtended & 0x00000000FFFFFFFF);

    }



    virtual void create() override;

};
