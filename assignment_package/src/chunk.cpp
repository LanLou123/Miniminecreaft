# include <scene/terrain.h>

const float gridOffset = 1.0f / 16.0f;
const float epsilonPadding = 1e-4f;

const BlockType invalidBlockType = (BlockType)0xFF;

struct uvGrid
{
    glm::vec2 squareUV[4];
    uvGrid(int gridX, int gridY) :
        squareUV
        {
            glm::vec2(gridX * gridOffset + epsilonPadding, gridY * gridOffset + epsilonPadding),
            glm::vec2((gridX + 1) * gridOffset - epsilonPadding, gridY * gridOffset + epsilonPadding),
            glm::vec2((gridX + 1) * gridOffset - epsilonPadding, (gridY + 1) * gridOffset - epsilonPadding),
            glm::vec2(gridX * gridOffset + epsilonPadding, (gridY + 1) * gridOffset - epsilonPadding)
        }
    {}
};

static const GLfloat cubeRadius = 1.0f;
static const GLfloat cubeHR = 0.5f * cubeRadius;

int64_t Chunk::getXZGlobalPositions()
{
    return this->xzGlobalPos;
}

Chunk* Chunk::getLeftAdjacent()
{
    xzCoords c = this->getXZCoordUnpacked(this->xzGlobalPos);
    c.x -= 16;
    return terrain->getChunkAt(c.x, c.z);
}

Chunk* Chunk::getRightAdjacent()
{
    xzCoords c = this->getXZCoordUnpacked(this->xzGlobalPos);
    c.x += 16;
    return terrain->getChunkAt(c.x, c.z);
}

Chunk* Chunk::getBackAdjacent()
{
    xzCoords c = this->getXZCoordUnpacked(this->xzGlobalPos);
    c.z -= 16;
    return terrain->getChunkAt(c.x, c.z);
}

Chunk* Chunk::getFrontAdjacent()
{
    xzCoords c = this->getXZCoordUnpacked(this->xzGlobalPos);
    c.z += 16;
    return terrain->getChunkAt(c.x, c.z);
}

Chunk::Chunk(OpenGLContext *parent, Terrain *terrain, int64_t xz) : Drawable(parent),
    xzGlobalPos(xz), terrain(terrain)
{
    memset(this->blocks, invalidBlockType, 65536);
}

int& Chunk::accessHeightAtGlobal(int x, int z)
{
    xzCoords coord = Chunk::getXZCoordUnpacked(this->xzGlobalPos);
    x -= coord.x;
    z -= coord.z;
    return this->accessHeightAt(x, z);
}

int& Chunk::accessHeightAt(size_t x, size_t z)
{
    return this->heightField[16 * x + z];
}

//#define BOUNDRYCHECK

BlockType Chunk::getBlockType(size_t x, size_t y, size_t z)
{
    return accessBlockType(x, y, z);
}

BlockType& Chunk::accessBlockType(size_t x, size_t y, size_t z)
{
    size_t index = 4096*x + 256*z + y;
#ifdef BOUNDRYCHECK
    if (index > 65535)
    {
        qWarning("Chunk access overflow");
    }
#endif
    if (blocks[index] == invalidBlockType)
    {
        int heightInt = this->accessHeightAt(x, z) - 1;
        if(y < 129)
        {
            blocks[index] = STONE;
        }
        else if(y < 129 + heightInt)
        {
            blocks[index] = DIRT;
        }
        else if(y == 129 + heightInt)
        {
            blocks[index] = GRASS;
        }
        else
        {
            blocks[index] = EMPTY;
        }
    }
    return blocks[index];
}

BlockType& Chunk::accessBlockTypeGlobalCoords(int x, int y, int z)
{
    xzCoords coord = Chunk::getXZCoordUnpacked(this->xzGlobalPos);
    x -= coord.x;
    z -= coord.z;
    return this->accessBlockType(x, y, z);
}

static const glm::vec2 grassTop[4] = uvGrid(8, 13).squareUV;
static const glm::vec2 grassSide[4] = uvGrid(3, 15).squareUV;
static const glm::vec2 dirt[4] = uvGrid(2, 15).squareUV;
static const glm::vec2 stone[4] = uvGrid(1, 15).squareUV;
static const glm::vec2 bedRock[4] = uvGrid(1, 14).squareUV;
static const glm::vec2 woodSide[4] = uvGrid(4, 14).squareUV;
static const glm::vec2 woodTopBot[4] = uvGrid(5, 14).squareUV;
static const glm::vec2 leaf[4] = uvGrid(5, 12).squareUV;
static const glm::vec2 sandBottom[4] = uvGrid(2, 14).squareUV;
static const glm::vec2 ice[4] = uvGrid(3, 11).squareUV;
static const glm::vec2 water[4] = uvGrid(14, 3).squareUV;
static const glm::vec2 lava[4] = uvGrid(14, 1).squareUV;
static const glm::vec2 gold[4] = uvGrid(0, 13).squareUV;
static const glm::vec2 ironore[4] = uvGrid(1, 13).squareUV;
static const glm::vec2 coal[4] = uvGrid(2, 13).squareUV;

static const glm::vec2 flowU = glm::vec2(1.0f, 0.0f);
static const glm::vec2 flowV = glm::vec2(0.0f, 1.0f);
static const glm::vec2 flowStatic = glm::vec2(0.0f, 0.0f);

void Chunk::appendUV(std::vector<GLfloat> *container, const glm::vec2 uvCoords[])
{
    for (unsigned i = 0; i!=4; ++i)
    {
        for (unsigned j = 0; j!=2; ++j)
        {
            container->push_back((*(uvCoords + i))[j]);
        }
    }
}

void Chunk::appendFlow(std::vector<GLfloat> *container, glm::vec2 speed)
{
    for (unsigned i = 0; i!=4; ++i)
    {
        for (unsigned j = 0; j!=2; ++j)
        {
            container->push_back(speed[j]);
        }
    }
}

void Chunk::fillFace(glm::vec4 positions[], glm::vec4 normal, BlockType type, FaceFacing facing)
{
    std::vector<GLfloat>* posC;
    std::vector<GLfloat>* norC;
    std::vector<GLfloat>* uvC;
    std::vector<GLfloat>* flowVelocityC;
    std::vector<GLuint>* eleC;
    std::vector<GLfloat>* tanC;
    std::vector<GLfloat>* bitanC;
    std::vector<GLint>* buftypeC;

    if (type == WATER)
    {
        posC = &this->posF;
        norC = &this->norF;
        uvC = &this->uvF;
        flowVelocityC = &this->flowVelocityF;
        eleC = &this->eleF;
        tanC = &this->tanF;
        bitanC = &this->bitanF;
        buftypeC = &this->buftypeF;
    }
    else
    {
        posC = &this->pos;
        norC = &this->nor;
        uvC = &this->uv;
        flowVelocityC = &this->flowVelocity;
        eleC = &this->ele;
        tanC = &this->tan;
        bitanC = &this->bitan;
        buftypeC = &this->buftype;
    }

    for (unsigned i = 0; i!=4; ++i)
    {
        for (unsigned j = 0; j!=4; ++j)
        {
            posC->push_back(positions[i][j]);
        }
    }
    for (unsigned i = 0; i!=4; ++i)
    {
        for (unsigned j = 0; j!=4; ++j)
        {
            norC->push_back(normal[j]);
        }
    }

    glm::vec2 deltaUV1;
    glm::vec2 deltaUV2;

    switch (type)
    {
    case GRASS:
        if (facing == UP)
        {
            appendUV(uvC, grassTop);
            deltaUV1 = grassTop[1] - grassTop[0];
            deltaUV2 = grassTop[3] - grassTop[0];
        }
        else if (facing == DOWN)
        {
            appendUV(uvC, dirt);
            deltaUV1 = dirt[1] - dirt[0];
            deltaUV2 = dirt[3] - dirt[0];
        }
        else
        {
            appendUV(uvC, grassSide);
            deltaUV1 = grassSide[1] - grassSide[0];
            deltaUV2 = grassSide[3] - grassSide[0];
        }
        appendFlow(flowVelocityC, flowStatic);
        break;
    case DIRT:
        appendUV(uvC, dirt);
        appendFlow(flowVelocityC, flowStatic);
        deltaUV1 = dirt[1] - dirt[0];
        deltaUV2 = dirt[3] - dirt[0];
        break;
    case STONE:
        appendUV(uvC, stone);
        appendFlow(flowVelocityC, flowStatic);
        deltaUV1 = stone[1] - stone[0];
        deltaUV2 = stone[3] - stone[0];
        break;
    case LAVA:
        appendUV(uvC, lava);
        appendFlow(flowVelocityC, flowU);
        deltaUV1 = lava[1] - lava[0];
        deltaUV2 = lava[3] - lava[0];
        break;
    case WATER:
        appendUV(uvC, water);
        appendFlow(flowVelocityC, flowU);
        deltaUV1 = water[1] - water[0];
        deltaUV2 = water[3] - water[0];
        break;
    case GOLD:
        appendUV(uvC, gold);
        appendFlow(flowVelocityC, flowStatic);
        deltaUV1 = gold[1] - gold[0];
        deltaUV2 = gold[3] - gold[0];
        break;
    case IRONORE:
        appendUV(uvC, ironore);
        appendFlow(flowVelocityC, flowStatic);
        deltaUV1 = ironore[1] - ironore[0];
        deltaUV2 = ironore[3] - ironore[0];
        break;
    case COAL:
        appendUV(uvC, coal);
        appendFlow(flowVelocityC, flowStatic);
        deltaUV1 = coal[1] - coal[0];
        deltaUV2 = coal[3] - coal[0];
        break;
    default:
        appendUV(uvC, stone);
        appendFlow(flowVelocityC, flowStatic);
        deltaUV1 = stone[1] - stone[0];
        deltaUV2 = stone[3] - stone[0];
        break;
    }

    glm::vec4 deltaPos1 = positions[1] - positions[0];
    glm::vec4 deltaPos2 = positions[3] - positions[0];
    glm::vec4 T = (deltaUV2.y * deltaPos1 - deltaUV1.y * deltaPos2) /
            (deltaUV1.x*deltaUV2.y - deltaUV2.x*deltaUV1.y);
    glm::vec4 B = (deltaUV2.x*deltaPos1 - deltaUV1.x*deltaPos2) /
            (deltaUV1.y*deltaUV2.x - deltaUV2.y*deltaUV1.x);

    for (unsigned i = 0; i!=4; ++i)
    {
        for (unsigned j = 0; j!=4; ++j)
        {
            tanC->push_back(T[j]);
            bitanC->push_back(B[j]);
        }
    }

    for (unsigned i = 0; i!=4; ++i)
    {
        buftypeC->push_back(type);
    }

    size_t indexoffset = eleC->size() / 3 * 2;
    eleC->push_back(indexoffset);
    eleC->push_back(indexoffset + 1);
    eleC->push_back(indexoffset + 2);
    eleC->push_back(indexoffset);
    eleC->push_back(indexoffset + 2);
    eleC->push_back(indexoffset + 3);
}

void Chunk::fillLeftFace(size_t x, size_t y, size_t z, BlockType type)
{
    if (x == 0)
    {
        //Check the chunk to its left
        Chunk* adjNegX = this->getLeftAdjacent();
        if (adjNegX != nullptr && !adjNegX->shouldFill(15, y, z, type))
        {
            return;
        }
    }
    //Check the block to its left
    else
    {
        if (!this->shouldFill(x - 1, y, z, type))
        {
            return;
        }
    }
    glm::vec4 square[4] =
    {
        glm::vec4(x - cubeHR, y - cubeHR, z - cubeHR, 1.0f),
        glm::vec4(x - cubeHR, y - cubeHR, z + cubeHR, 1.0f),
        glm::vec4(x - cubeHR, y + cubeHR, z + cubeHR, 1.0f),
        glm::vec4(x - cubeHR, y + cubeHR, z - cubeHR, 1.0f)
    };
    xzCoords xzCoordinate = this->getXZCoordUnpacked(this->xzGlobalPos);
    glm::vec4 offset = glm::vec4(xzCoordinate.x, 0.0f, xzCoordinate.z, 0.0f);

    for (unsigned i = 0; i!=4; ++i)
    {
        square[i] += offset;
    }

    glm::vec4 normal = glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f);
    this->fillFace(square, normal, type, LEFT);
}

void Chunk::fillRightFace(size_t x, size_t y, size_t z, BlockType type)
{
    //Check right
    if (x == 15)
    {
        //Checkthe chunk to its right
        Chunk* adjPosX = this->getRightAdjacent();
        if (adjPosX != nullptr && !adjPosX->shouldFill(0, y, z, type))
        {
            return;
        }
    }
    //Check the block to its right
    else
    {
        if (!this->shouldFill(x + 1, y, z, type))
        {
            return;
        }
    }

    glm::vec4 square[4] =
    {
        glm::vec4(x + cubeHR, y - cubeHR, z - cubeHR, 1.0f),
        glm::vec4(x + cubeHR, y - cubeHR, z + cubeHR, 1.0f),
        glm::vec4(x + cubeHR, y + cubeHR, z + cubeHR, 1.0f),
        glm::vec4(x + cubeHR, y + cubeHR, z - cubeHR, 1.0f)
    };
    xzCoords xzCoordinate = this->getXZCoordUnpacked(this->xzGlobalPos);
    glm::vec4 offset = glm::vec4(xzCoordinate.x, 0.0f, xzCoordinate.z, 0.0f);

    for (unsigned i = 0; i!=4; ++i)
    {
        square[i] += offset;
    }

    glm::vec4 normal = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
    this->fillFace(square, normal, type, RIGHT);
}

void Chunk::fillUpFace(size_t x, size_t y, size_t z, BlockType type)
{
    if (y != 255 && !this->shouldFill(x, y + 1, z, type))
    {
        return;
    }

    glm::vec4 square[4] =
    {
        glm::vec4(x - cubeHR, y + cubeHR, z - cubeHR, 1.0f),
        glm::vec4(x + cubeHR, y + cubeHR, z - cubeHR, 1.0f),
        glm::vec4(x + cubeHR, y + cubeHR, z + cubeHR, 1.0f),
        glm::vec4(x - cubeHR, y + cubeHR, z + cubeHR, 1.0f)
    };
    xzCoords xzCoordinate = this->getXZCoordUnpacked(this->xzGlobalPos);
    glm::vec4 offset = glm::vec4(xzCoordinate.x, 0.0f, xzCoordinate.z, 0.0f);

    for (unsigned i = 0; i!=4; ++i)
    {
        square[i] += offset;
    }

    glm::vec4 normal = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
    this->fillFace(square, normal, type, UP);
}

void Chunk::fillDownFace(size_t x, size_t y, size_t z, BlockType type)
{
    if (y != 0 && !this->shouldFill(x, y - 1, z, type))
    {
        return;
    }

    glm::vec4 square[4] =
    {
        glm::vec4(x - cubeHR, y - cubeHR, z - cubeHR, 1.0f),
        glm::vec4(x + cubeHR, y - cubeHR, z - cubeHR, 1.0f),
        glm::vec4(x + cubeHR, y - cubeHR, z + cubeHR, 1.0f),
        glm::vec4(x - cubeHR, y - cubeHR, z + cubeHR, 1.0f)
    };
    xzCoords xzCoordinate = this->getXZCoordUnpacked(this->xzGlobalPos);
    glm::vec4 offset = glm::vec4(xzCoordinate.x, 0.0f, xzCoordinate.z, 0.0f);

    for (unsigned i = 0; i!=4; ++i)
    {
        square[i] += offset;
    }

    glm::vec4 normal = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
    this->fillFace(square, normal, type, DOWN);
}

void Chunk::fillFrontFace(size_t x, size_t y, size_t z, BlockType type)
{
    //Check front
    if (z == 15)
    {
        //Check the chunk to its front
        Chunk* adjPosZ = this->getFrontAdjacent();
        if (adjPosZ != nullptr && !adjPosZ->shouldFill(x, y, 0, type))
        {
            return;
        }
    }
    //Check the block to its front
    else
    {
        if (!this->shouldFill(x, y, z + 1, type))
        {
            return;
        }
    }

    glm::vec4 square[4] =
    {
        glm::vec4(x - cubeHR, y - cubeHR, z + cubeHR, 1.0f),
        glm::vec4(x + cubeHR, y - cubeHR, z + cubeHR, 1.0f),
        glm::vec4(x + cubeHR, y + cubeHR, z + cubeHR, 1.0f),
        glm::vec4(x - cubeHR, y + cubeHR, z + cubeHR, 1.0f)
    };
    xzCoords xzCoordinate = this->getXZCoordUnpacked(this->xzGlobalPos);
    glm::vec4 offset = glm::vec4(xzCoordinate.x, 0.0f, xzCoordinate.z, 0.0f);

    for (unsigned i = 0; i!=4; ++i)
    {
        square[i] += offset;
    }

    glm::vec4 normal = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
    this->fillFace(square, normal, type, FRONT);
}

void Chunk::fillBackFace(size_t x, size_t y, size_t z, BlockType type)
{
    //Check back
    if (z == 0)
    {
        //Check the chunk to its back
        Chunk* adjNegZ = this->getBackAdjacent();
        if (adjNegZ != nullptr && !adjNegZ->shouldFill(x, y, 15, type))
        {
            return;
        }
    }
    //Check the block to its back
    else
    {
        if (!this->shouldFill(x, y, z - 1, type))
        {
            return;
        }
    }

    glm::vec4 square[4] =
    {
        glm::vec4(x - cubeHR, y - cubeHR, z - cubeHR, 1.0f),
        glm::vec4(x + cubeHR, y - cubeHR, z - cubeHR, 1.0f),
        glm::vec4(x + cubeHR, y + cubeHR, z - cubeHR, 1.0f),
        glm::vec4(x - cubeHR, y + cubeHR, z - cubeHR, 1.0f)
    };
    xzCoords xzCoordinate = this->getXZCoordUnpacked(this->xzGlobalPos);
    glm::vec4 offset = glm::vec4(xzCoordinate.x, 0.0f, xzCoordinate.z, 0.0f);

    for (unsigned i = 0; i!=4; ++i)
    {
        square[i] += offset;
    }

    glm::vec4 normal = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
    this->fillFace(square, normal, type, BACK);
}

bool Chunk::shouldFill(size_t x, size_t y, size_t z, BlockType currentBlock)
{
    BlockType probingBlock = this->getBlockType(x, y, z);
    if ((currentBlock == LAVA || currentBlock == WATER)
            && currentBlock == probingBlock)
    {
        return false;
    }
    if (probingBlock == EMPTY || probingBlock == LAVA || probingBlock == WATER)
    {
        return true;
    }
    else
    {
        return false;
    }
}

size_t size2Reserve4components = 65536 / 2 * 6 * 4 * 4;
size_t size2Reserve2components = 65536 / 2 * 6 * 4 * 2;
size_t size2Reserve1component = 65536 / 2 * 6 * 4 * 1;
size_t size2ReserveElement = 65536 / 2 * 6 * 6;

void Chunk::create()
{
    this->pos.clear();
    this->nor.clear();
    this->uv.clear();
    this->flowVelocity.clear();
    this->ele.clear();
    this->tan.clear();
    this->bitan.clear();
    this->buftype.clear();

    this->posF.clear();
    this->norF.clear();
    this->uvF.clear();
    this->flowVelocityF.clear();
    this->eleF.clear();
    this->tanF.clear();
    this->bitanF.clear();
    this->buftypeF.clear();


    this->pos.reserve(size2Reserve4components);
    this->nor.reserve(size2Reserve4components);
    this->uv.reserve(size2Reserve2components);
    this->flowVelocity.reserve(size2Reserve2components);
    this->ele.reserve(size2ReserveElement);
    this->tan.reserve(size2Reserve4components);
    this->bitan.reserve(size2Reserve4components);
    this->buftype.reserve(size2Reserve1component);

    this->posF.reserve(size2Reserve4components);
    this->norF.reserve(size2Reserve4components);
    this->uvF.reserve(size2Reserve2components);
    this->flowVelocityF.reserve(size2Reserve2components);
    this->eleF.reserve(size2ReserveElement);
    this->tanF.reserve(size2Reserve4components);
    this->bitanF.reserve(size2Reserve4components);
    this->buftypeF.reserve(size2Reserve1component);

    for (size_t x = 0; x != 16; ++x)
    {
        for (size_t z = 0; z != 16; ++z)
        {
            for (size_t y = 0; y != 256; ++y)
            {
                BlockType currentBlock = this->getBlockType(x, y, z);
                if (currentBlock == EMPTY)
                {
                    continue;
                }

                fillLeftFace(x, y, z, this->getBlockType(x, y, z));
                fillRightFace(x, y, z, this->getBlockType(x, y, z));
                fillBackFace(x, y, z, this->getBlockType(x, y, z));
                fillFrontFace(x, y, z, this->getBlockType(x, y, z));
                fillUpFace(x, y, z, this->getBlockType(x, y, z));
                fillDownFace(x, y, z, this->getBlockType(x, y, z));
            }
        }
    }

    this->count = ele.size();

    generateIdx();
    context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * ele.size(),
                             reinterpret_cast<void*>(ele.data()), GL_STATIC_DRAW);

    generatePos();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * pos.size(),
                             reinterpret_cast<void*>(pos.data()), GL_STATIC_DRAW);

    generateNor();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufNor);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * nor.size(),
                             reinterpret_cast<void*>(nor.data()), GL_STATIC_DRAW);

    generateUV();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufUV);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * uv.size(),
                            reinterpret_cast<void*>(uv.data()), GL_STATIC_DRAW);

    generateFlowVelocity();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufFlowVelocity);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * flowVelocity.size(),
                            reinterpret_cast<void*>(flowVelocity.data()), GL_STATIC_DRAW);

    generateTangent();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufTangent);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * tan.size(),
                            reinterpret_cast<void*>(tan.data()), GL_STATIC_DRAW);

    generateBiTangent();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufBiTangent);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * bitan.size(),
                            reinterpret_cast<void*>(bitan.data()), GL_STATIC_DRAW);

    generateBlockType();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufBlockType);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(GLint) * buftype.size(),
                            reinterpret_cast<void*>(buftype.data()), GL_STATIC_DRAW);


    this->countF = eleF.size();

    generateIdxF();
    context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdxF);
    context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * eleF.size(),
                             reinterpret_cast<void*>(eleF.data()), GL_STATIC_DRAW);

    generatePosF();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufPosF);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * posF.size(),
                             reinterpret_cast<void*>(posF.data()), GL_STATIC_DRAW);

    generateNorF();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufNorF);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * norF.size(),
                             reinterpret_cast<void*>(norF.data()), GL_STATIC_DRAW);

    generateUVF();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufUVF);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * uvF.size(),
                            reinterpret_cast<void*>(uvF.data()), GL_STATIC_DRAW);

    generateFlowVelocityF();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufFlowVelocityF);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * flowVelocityF.size(),
                            reinterpret_cast<void*>(flowVelocityF.data()), GL_STATIC_DRAW);

    generateTangentF();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufTangentF);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * tanF.size(),
                            reinterpret_cast<void*>(tanF.data()), GL_STATIC_DRAW);

    generateBiTangentF();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufBiTangentF);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * bitanF.size(),
                            reinterpret_cast<void*>(bitanF.data()), GL_STATIC_DRAW);

    generateBlockTypeF();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufBlockTypeF);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(GLint) * buftypeF.size(),
                            reinterpret_cast<void*>(buftypeF.data()), GL_STATIC_DRAW);
}
