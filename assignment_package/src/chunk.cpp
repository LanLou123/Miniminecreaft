# include <scene/terrain.h>

const float gridOffset = 1.0f / 16.0f;
const float epsilonPadding = 1e-4f;

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
}

#define BOUNDRYCHECK

BlockType Chunk::getBlockType(size_t x, size_t y, size_t z) const
{
    size_t index = 4096*x + 256*z + y;
#ifdef BOUNDRYCHECK
    if (index > 65535)
    {
        qWarning("Chunk access overflow");
    }
#endif
    return this->blocks[index];
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
    return this->blocks[4096*x + 256*z + y];
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

static const glm::vec2 flowU = glm::vec2(1.0f, 0.0f);
static const glm::vec2 flowV = glm::vec2(0.0f, 1.0f);
static const glm::vec2 flowStatic = glm::vec2(0.0f, 0.0f);

void Chunk::appendUV(GLfloat *container, const glm::vec2 uvCoords[], bool isOpaque)
{
    if (isOpaque)
    {
        for (unsigned i = 0; i!=4; ++i)
        {
            for (unsigned j = 0; j!=2; ++j)
            {
                container[uvSize] = (*(uvCoords + i))[j];
                uvSize++;
            }
        }
    }
    else
    {
        for (unsigned i = 0; i!=4; ++i)
        {
            for (unsigned j = 0; j!=2; ++j)
            {
                container[uvSizeF] = (*(uvCoords + i))[j];
                uvSizeF++;
            }
        }
    }
}

void Chunk::appendFlow(GLfloat *container, glm::vec2 speed, bool isOpaque)
{
    if (isOpaque)
    {
        for (unsigned i = 0; i!=4; ++i)
        {
            for (unsigned j = 0; j!=2; ++j)
            {
                container[flowVelocitySize] = speed[j];
                flowVelocitySize++;
            }
        }
    }
    else
    {
        for (unsigned i = 0; i!=4; ++i)
        {
            for (unsigned j = 0; j!=2; ++j)
            {
                container[flowVelocitySizeF] = speed[j];
                flowVelocityF++;
            }
        }
    }
}

void Chunk::fillFace(glm::vec4 positions[], glm::vec4 normal, BlockType type, FaceFacing facing)
{
    GLfloat* posC;
    GLfloat* norC;
    GLfloat* uvC;
    GLfloat* flowVelocityC;
    GLuint* eleC;
    GLfloat* tanC;
    GLfloat* bitanC;
    GLint* buftypeC;

    bool isOpaque;

    if (type == WATER)
    {
        posC = this->posF;
        norC = this->norF;
        uvC = this->uvF;
        flowVelocityC = this->flowVelocityF;
        eleC = this->eleF;
        tanC = this->tanF;
        bitanC = this->bitanF;
        buftypeC = this->buftypeF;

        isOpaque = false;
    }
    else
    {
        posC = this->pos;
        norC = this->nor;
        uvC = this->uv;
        flowVelocityC = this->flowVelocity;
        eleC = this->ele;
        tanC = this->tan;
        bitanC = this->bitan;
        buftypeC = this->buftype;

        isOpaque = true;
    }

    if (isOpaque)
    {
        for (unsigned i = 0; i!=4; ++i)
        {
            for (unsigned j = 0; j!=4; ++j)
            {
                posC[posSize] = positions[i][j];
                ++posSize;
            }
        }
        for (unsigned i = 0; i!=4; ++i)
        {
            for (unsigned j = 0; j!=4; ++j)
            {
                norC[norSize] = normal[j];
                ++norSize;
            }
        }
    }
    else
    {
        for (unsigned i = 0; i!=4; ++i)
        {
            for (unsigned j = 0; j!=4; ++j)
            {
                posC[posSizeF] = positions[i][j];
                ++posSizeF;
            }
        }
        for (unsigned i = 0; i!=4; ++i)
        {
            for (unsigned j = 0; j!=4; ++j)
            {
                norC[norSizeF] = normal[j];
                ++norSizeF;
            }
        }
    }

    glm::vec2 deltaUV1;
    glm::vec2 deltaUV2;

    switch (type)
    {
    case GRASS:
        if (facing == UP)
        {
            appendUV(uvC, grassTop, isOpaque);
            deltaUV1 = grassTop[1] - grassTop[0];
            deltaUV2 = grassTop[3] - grassTop[0];
        }
        else if (facing == DOWN)
        {
            appendUV(uvC, dirt, isOpaque);
            deltaUV1 = dirt[1] - dirt[0];
            deltaUV2 = dirt[3] - dirt[0];
        }
        else
        {
            appendUV(uvC, grassSide, isOpaque);
            deltaUV1 = grassSide[1] - grassSide[0];
            deltaUV2 = grassSide[3] - grassSide[0];
        }
        appendFlow(flowVelocityC, flowStatic, isOpaque);
        break;
    case DIRT:
        appendUV(uvC, dirt, isOpaque);
        appendFlow(flowVelocityC, flowStatic, isOpaque);
        deltaUV1 = dirt[1] - dirt[0];
        deltaUV2 = dirt[3] - dirt[0];
        break;
    case STONE:
        appendUV(uvC, stone, isOpaque);
        appendFlow(flowVelocityC, flowStatic, isOpaque);
        deltaUV1 = stone[1] - stone[0];
        deltaUV2 = stone[3] - stone[0];
        break;
    case LAVA:
        appendUV(uvC, lava, isOpaque);
        appendFlow(flowVelocityC, flowU, isOpaque);
        deltaUV1 = lava[1] - lava[0];
        deltaUV2 = lava[3] - lava[0];
        break;
    case WATER:
        appendUV(uvC, water, isOpaque);
        appendFlow(flowVelocityC, flowU, isOpaque);
        deltaUV1 = water[1] - water[0];
        deltaUV2 = water[3] - water[0];
        break;
    default:
        appendUV(uvC, stone, isOpaque);
        appendFlow(flowVelocityC, flowStatic, isOpaque);
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

    if (isOpaque)
    {
        for (unsigned i = 0; i!=4; ++i)
        {
            for (unsigned j = 0; j!=4; ++j)
            {
                tanC[tanSize] = T[j];
                bitanC[bitanSize] = B[j];
                tanSize++;
                bitanSize++;
            }
        }

        for (unsigned i = 0; i!=4; ++i)
        {
            buftypeC[buftypeSize] = type;
            ++buftypeSize;
        }

        size_t indexoffset = eleSize / 3 * 2;
        eleC[eleSize] = (indexoffset);
        eleSize++;
        eleC[eleSize] = indexoffset + 1;
        eleSize++;
        eleC[eleSize] = indexoffset + 2;
        eleSize++;
        eleC[eleSize] = indexoffset;
        eleSize++;
        eleC[eleSize] = indexoffset + 2;
        eleSize++;
        eleC[eleSize] = indexoffset + 3;
        eleSize++;
    }
    else
    {
        for (unsigned i = 0; i!=4; ++i)
        {
            for (unsigned j = 0; j!=4; ++j)
            {
                tanC[tanSizeF] = T[j];
                bitanC[bitanSizeF] = B[j];
                tanSizeF++;
                bitanSizeF++;
            }
        }

        for (unsigned i = 0; i!=4; ++i)
        {
            buftypeC[buftypeSizeF] = type;
            ++buftypeSizeF;
        }

        size_t indexoffset = eleSizeF / 3 * 2;
        eleC[eleSizeF] = (indexoffset);
        eleSizeF++;
        eleC[eleSizeF] = indexoffset + 1;
        eleSizeF++;
        eleC[eleSizeF] = indexoffset + 2;
        eleSizeF++;
        eleC[eleSizeF] = indexoffset;
        eleSizeF++;
        eleC[eleSizeF] = indexoffset + 2;
        eleSizeF++;
        eleC[eleSizeF] = indexoffset + 3;
        eleSizeF++;
    }
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
    posSize = posSizeF = norSize = norSizeF = uvSize = uvSizeF =
    flowVelocitySize = flowVelocitySizeF = eleSize = eleSizeF = tanSize =
    tanSizeF = bitanSize = bitanSizeF = buftypeSize = buftypeSizeF = 0;


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

    this->count = eleSize;

    generateIdx();
    context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * eleSize,
                             reinterpret_cast<void*>(ele), GL_STATIC_DRAW);

    generatePos();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * posSize,
                             reinterpret_cast<void*>(pos), GL_STATIC_DRAW);

    generateNor();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufNor);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * norSize,
                             reinterpret_cast<void*>(nor), GL_STATIC_DRAW);

    generateUV();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufUV);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * uvSize,
                            reinterpret_cast<void*>(uv), GL_STATIC_DRAW);

    generateFlowVelocity();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufFlowVelocity);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * flowVelocitySize,
                            reinterpret_cast<void*>(flowVelocity), GL_STATIC_DRAW);

    generateTangent();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufTangent);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * tanSize,
                            reinterpret_cast<void*>(tan), GL_STATIC_DRAW);

    generateBiTangent();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufBiTangent);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * bitanSize,
                            reinterpret_cast<void*>(bitan), GL_STATIC_DRAW);

    generateBlockType();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufBlockType);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(GLint) * buftypeSize,
                            reinterpret_cast<void*>(buftype), GL_STATIC_DRAW);


    this->countF = eleSizeF;

    generateIdxF();
    context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdxF);
    context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * eleSizeF,
                             reinterpret_cast<void*>(eleF), GL_STATIC_DRAW);

    generatePosF();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufPosF);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * posSizeF,
                             reinterpret_cast<void*>(posF), GL_STATIC_DRAW);

    generateNorF();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufNorF);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * norSizeF,
                             reinterpret_cast<void*>(norF), GL_STATIC_DRAW);

    generateUVF();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufUVF);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * uvSizeF,
                            reinterpret_cast<void*>(uvF), GL_STATIC_DRAW);

    generateFlowVelocityF();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufFlowVelocityF);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * flowVelocitySizeF,
                            reinterpret_cast<void*>(flowVelocityF), GL_STATIC_DRAW);

    generateTangentF();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufTangentF);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * tanSizeF,
                            reinterpret_cast<void*>(tanF), GL_STATIC_DRAW);

    generateBiTangentF();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufBiTangentF);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * bitanSizeF,
                            reinterpret_cast<void*>(bitanF), GL_STATIC_DRAW);

    generateBlockTypeF();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufBlockTypeF);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(GLint) * buftypeSizeF,
                            reinterpret_cast<void*>(buftypeF), GL_STATIC_DRAW);
}
