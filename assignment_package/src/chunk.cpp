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

BlockType Chunk::getBlockType(size_t x, size_t y, size_t z) const
{
    return this->blocks[4096*x + 256*z + y];
}

BlockType& Chunk::accessBlockType(size_t x, size_t y, size_t z)
{
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

void Chunk::appendUV(const glm::vec2 uvCoords[])
{
    for (unsigned i = 0; i!=4; ++i)
    {
        for (unsigned j = 0; j!=2; ++j)
        {
            uv.push_back((*(uvCoords + i))[j]);
        }
    }
}

void Chunk::appendFlow(glm::vec2 speed)
{
    for (unsigned i = 0; i!=4; ++i)
    {
        for (unsigned j = 0; j!=2; ++j)
        {
            flowVelocity.push_back(speed[j]);
        }
    }
}

void Chunk::fillFace(glm::vec4 positions[], glm::vec4 normal, BlockType type, FaceFacing facing)
{
    for (unsigned i = 0; i!=4; ++i)
    {
        for (unsigned j = 0; j!=4; ++j)
        {
            pos.push_back(positions[i][j]);
        }
    }
    for (unsigned i = 0; i!=4; ++i)
    {
        for (unsigned j = 0; j!=4; ++j)
        {
            nor.push_back(normal[j]);
        }
    }

    glm::vec2 deltaUV1;
    glm::vec2 deltaUV2;

    switch (type)
    {
    case GRASS:
        if (facing == UP)
        {
            appendUV(grassTop);
            deltaUV1 = grassTop[1] - grassTop[0];
            deltaUV2 = grassTop[3] - grassTop[0];
        }
        else if (facing == DOWN)
        {
            appendUV(dirt);
            deltaUV1 = dirt[1] - dirt[0];
            deltaUV2 = dirt[3] - dirt[0];
        }
        else
        {
            appendUV(grassSide);
            deltaUV1 = grassSide[1] - grassSide[0];
            deltaUV2 = grassSide[3] - grassSide[0];
        }
        appendFlow(flowStatic);
        break;
    case DIRT:
        appendUV(dirt);
        appendFlow(flowStatic);
        deltaUV1 = dirt[1] - dirt[0];
        deltaUV2 = dirt[3] - dirt[0];
        break;
    case STONE:
        appendUV(stone);
        appendFlow(flowStatic);
        deltaUV1 = stone[1] - stone[0];
        deltaUV2 = stone[3] - stone[0];
        break;
    case LAVA:
        appendUV(lava);
        appendFlow(flowU);
        deltaUV1 = lava[1] - lava[0];
        deltaUV2 = lava[3] - lava[0];
        break;
    case WATER:
        appendUV(water);
        appendFlow(flowU);
        deltaUV1 = water[1] - water[0];
        deltaUV2 = water[3] - water[0];
        break;
    default:
        appendUV(stone);
        appendFlow(flowStatic);
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
            tan.push_back(T[j]);
            bitan.push_back(B[j]);
        }
    }

    size_t indexoffset = ele.size() / 3 * 2;
    ele.push_back(indexoffset);
    ele.push_back(indexoffset + 1);
    ele.push_back(indexoffset + 2);
    ele.push_back(indexoffset);
    ele.push_back(indexoffset + 2);
    ele.push_back(indexoffset + 3);
}

void Chunk::fillLeftFace(size_t x, size_t y, size_t z, BlockType type)
{
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

void Chunk::create()
{
    this->pos.clear();
    this->nor.clear();
    this->uv.clear();
    this->flowVelocity.clear();
    this->ele.clear();
    this->tan.clear();
    this->bitan.clear();
    for (size_t x = 0; x != 16; ++x)
    {
        for (size_t z = 0; z != 16; ++z)
        {
            for (size_t y = 0; y != 256; ++y)
            {
                if (this->getBlockType(x, y, z) == EMPTY)
                {
                    continue;
                }

                //Check left
                if (x == 0)
                {
                    //Check the chunk to its left
                    Chunk* adjNegX = this->getLeftAdjacent();
                    if (adjNegX == nullptr || adjNegX->shouldFill(15, y, z))
                    {
                        fillLeftFace(x, y, z, this->getBlockType(x, y, z));
                    }
                }
                //Check the block to its left
                else
                {
                    if (this->shouldFill(x - 1, y, z))
                    {
                        fillLeftFace(x, y, z, this->getBlockType(x, y, z));
                    }
                }


                //Check right
                if (x == 15)
                {
                    //Checkthe chunk to its right
                    Chunk* adjPosX = this->getRightAdjacent();
                    if (adjPosX == nullptr || adjPosX->shouldFill(0, y, z))
                    {
                        fillRightFace(x, y, z, this->getBlockType(x, y, z));
                    }
                }
                //Check the block to its right
                else
                {
                    if (this->shouldFill(x + 1, y, z))
                    {
                        fillRightFace(x, y, z, this->getBlockType(x, y, z));
                    }
                }


                //Check back
                if (z == 0)
                {
                    //Check the chunk to its back
                    Chunk* adjNegZ = this->getBackAdjacent();
                    if (adjNegZ == nullptr || adjNegZ->shouldFill(x, y, 15))
                    {
                        fillBackFace(x, y, z, this->getBlockType(x, y, z));
                    }
                }
                //Check the block to its back
                else
                {
                    if (this->shouldFill(x, y, z - 1))
                    {
                        fillBackFace(x, y, z, this->getBlockType(x, y, z));
                    }
                }


                //Check front
                if (z == 15)
                {
                    //Check the chunk to its front
                    Chunk* adjPosZ = this->getFrontAdjacent();
                    if (adjPosZ == nullptr || adjPosZ->shouldFill(x, y, 0))
                    {
                        fillFrontFace(x, y, z, this->getBlockType(x, y, z));
                    }
                }
                //Check the block to its front
                else
                {
                    if (this->shouldFill(x, y, z + 1))
                    {
                        fillFrontFace(x, y, z, this->getBlockType(x, y, z));
                    }
                }

                //Check up
                //Check the block above it
                if (y == 255 || this->shouldFill(x, y + 1, z))
                {
                    fillUpFace(x, y, z, this->getBlockType(x, y, z));
                }

                //Check down
                //Check the block below it
                if (y == 0 || this->shouldFill(x, y - 1, z))
                {
                    fillDownFace(x, y, z, this->getBlockType(x, y, z));
                }
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
}
