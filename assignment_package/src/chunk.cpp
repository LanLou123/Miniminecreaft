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
static const glm::vec2 water[4] = uvGrid(13, 3).squareUV;
static const glm::vec2 lava[4] = uvGrid(13, 1).squareUV;

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

    const glm::vec2* ptr2UVSquare;

    switch (type)
    {
    case GRASS:
        if (facing == UP)
        {
            ptr2UVSquare = grassTop;
        }
        else
        {
            ptr2UVSquare = grassSide;
        }
        break;
    case DIRT:
        ptr2UVSquare = dirt;
        break;
    case STONE:
        ptr2UVSquare = stone;
        break;
    case LAVA:
        ptr2UVSquare = lava;
        break;
    case WATER:
        ptr2UVSquare = water;
        break;
    default:
        ptr2UVSquare = leaf;
        break;
    }
    for (unsigned i = 0; i!=4; ++i)
    {
        for (unsigned j = 0; j!=2; ++j)
        {
            uv.push_back((*(ptr2UVSquare + i))[j]);
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

void Chunk::create()
{
    this->pos.clear();
    this->nor.clear();
    this->uv.clear();
    this->flowFlag.clear();
    this->ele.clear();
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
                    if (adjNegX == nullptr || adjNegX->getBlockType(15, y, z) == EMPTY)
                    {
                        fillLeftFace(x, y, z, this->getBlockType(x, y, z));
                    }
                }
                //Check the block to its left
                else
                {
                    if (this->getBlockType(x - 1, y, z) == EMPTY)
                    {
                        fillLeftFace(x, y, z, this->getBlockType(x, y, z));
                    }
                }


                //Check right
                if (x == 15)
                {
                    //Checkthe chunk to its right
                    Chunk* adjPosX = this->getRightAdjacent();
                    if (adjPosX == nullptr || adjPosX->getBlockType(0, y, z) == EMPTY)
                    {
                        fillRightFace(x, y, z, this->getBlockType(x, y, z));
                    }
                }
                //Check the block to its right
                else
                {
                    if (this->getBlockType(x + 1, y, z) == EMPTY)
                    {
                        fillRightFace(x, y, z, this->getBlockType(x, y, z));
                    }
                }


                //Check back
                if (z == 0)
                {
                    //Check the chunk to its back
                    Chunk* adjNegZ = this->getBackAdjacent();
                    if (adjNegZ == nullptr || adjNegZ->getBlockType(x, y, 15) == EMPTY)
                    {
                        fillBackFace(x, y, z, this->getBlockType(x, y, z));
                    }
                }
                //Check the block to its back
                else
                {
                    if (this->getBlockType(x, y, z - 1) == EMPTY)
                    {
                        fillBackFace(x, y, z, this->getBlockType(x, y, z));
                    }
                }


                //Check front
                if (z == 15)
                {
                    //Check the chunk to its front
                    Chunk* adjPosZ = this->getFrontAdjacent();
                    if (adjPosZ == nullptr || adjPosZ->getBlockType(x, y, 0) == EMPTY)
                    {
                        fillFrontFace(x, y, z, this->getBlockType(x, y, z));
                    }
                }
                //Check the block to its front
                else
                {
                    if (this->getBlockType(x, y, z + 1) == EMPTY)
                    {
                        fillFrontFace(x, y, z, this->getBlockType(x, y, z));
                    }
                }

                //Check up
                //Check the block above it
                if (y == 255 || this->getBlockType(x, y + 1, z) == EMPTY)
                {
                    fillUpFace(x, y, z, this->getBlockType(x, y, z));
                }

                //Check down
                //Check the block below it
                if (y == 0 || this->getBlockType(x, y - 1, z) == EMPTY)
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

    generateFlow();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufFlow);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(GLint) * flowFlag.size(),
                            reinterpret_cast<void*>(flowFlag.data()), GL_STATIC_DRAW);
}
