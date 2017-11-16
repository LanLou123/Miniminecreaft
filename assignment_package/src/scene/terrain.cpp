#include <scene/terrain.h>

#include <scene/cube.h>

Terrain::Terrain() : dimensions(64, 256, 64)
{}

BlockType Terrain::getBlockAt(int x, int y, int z) const
{
    // TODO: Make this work with your new block storage!
    return m_blocks[x][y][z];
}

void Terrain::setBlockAt(int x, int y, int z, BlockType t)
{
    // TODO: Make this work with your new block storage!
    m_blocks[x][y][z] = t;
}

void Terrain::CreateTestScene()
{
    // Create the basic terrain floor
    for(int x = 0; x < 64; ++x)
    {
        for(int z = 0; z < 64; ++z)
        {
            for(int y = 127; y < 256; ++y)
            {
                if(y <= 128)
                {
                    if((x + z) % 2 == 0)
                    {
                        m_blocks[x][y][z] = STONE;
                    }
                    else
                    {
                        m_blocks[x][y][z] = DIRT;
                    }
                }
                else
                {
                    m_blocks[x][y][z] = EMPTY;
                }
            }
        }
    }
    // Add "walls" for collision testing
    for(int x = 0; x < 64; ++x)
    {
        m_blocks[x][129][0] = GRASS;
        m_blocks[x][130][0] = GRASS;
        m_blocks[x][129][63] = GRASS;
        m_blocks[0][130][x] = GRASS;
    }
    for(int y = 129; y < 140; ++y)
    {
        m_blocks[32][y][32] = GRASS;
    }
}


Chunk::Chunk(OpenGLContext *parent, uint64_t xz) : Drawable(parent), xzGlobalPos(xz),
    adjPosZ(nullptr), adjNegZ(nullptr), adjPosX(nullptr), adjNegX(nullptr)
{
}

BlockType Chunk::getBlockType(size_t x, size_t y, size_t z) const
{
    return this->blocks[4096*z + 16*y + x];
}

BlockType& Chunk::accessBlockType(size_t x, size_t y, size_t z)
{
    return this->blocks[4096*z + 16*y + x];
}

void Chunk::setAdjPosZ(Chunk *ptr)
{
    this->adjPosZ = ptr;
}

void Chunk::setAdjNegZ(Chunk *ptr)
{
    this->adjNegZ = ptr;
}

void Chunk::setAdjPosX(Chunk *ptr)
{
    this->adjPosX = ptr;
}

void Chunk::setAdjNegX(Chunk *ptr)
{
    this->adjNegX = ptr;
}

static const GLfloat dirtColor[4] = {0.475f, 0.333f, 0.227f, 1.0f};
static const GLfloat grassColor[4] = {0.373f, 0.624f, 0.208f, 1.0f};
static const GLfloat stoneColor[4] = {0.5f, 0.5f, 0.5f, 1.0f};
const GLfloat cubeRadius = 1.0f;

void Chunk::fillLeftFace(size_t x, size_t y, size_t z, BlockType type)
{
    glm::vec4 square[4] =
    {
        glm::vec4(x, y, z, 1.0f),
        glm::vec4(x, y, z + cubeRadius, 1.0f),
        glm::vec4(x, y + cubeRadius, z + cubeRadius, 1.0f),
        glm::vec4(x, y + cubeRadius, z, 1.0f)
    };
    glm::vec4 offset = glm::vec4(this->xzGlobalPos >> 32, 0.0f, this->xzGlobalPos & 0xFFFFFFFF, 1.0f);

    for (unsigned i = 0; i!=4; ++i)
    {
        square[i] += offset;
        for (unsigned j = 0; j!=4; ++j)
        {
            pos.push_back(square[i][j]);
        }
    }

    glm::vec4 normal = glm::vec4(-1.0f, 0.0f, 0.0f, 1.0f);
    for (unsigned i = 0; i!=4; ++i)
    {
        nor.push_back(normal[i]);
    }

    switch (type)
    {
    case GRASS:
        for (unsigned i = 0; i!=4; ++i)
        {
            for (unsigned j = 0; j!=4; ++j)
            {
                col.push_back(grassColor[j]);
            }
        }
        break;
    case DIRT:
        for (unsigned i = 0; i!=4; ++i)
        {
            for (unsigned j = 0; j!=4; ++j)
            {
                col.push_back(dirtColor[j]);
            }
        }
        break;
    case STONE:
        for (unsigned i = 0; i!=4; ++i)
        {
            for (unsigned j = 0; j!=4; ++j)
            {
                col.push_back(stoneColor[j]);
            }
        }
        break;
    default:
        break;
    }

    size_t indexoffset = ele.size() / 3 * 2;
    ele.push_back(indexoffset);
    ele.push_back(indexoffset + 1);
    ele.push_back(indexoffset + 2);
    ele.push_back(indexoffset);
    ele.push_back(indexoffset + 2);
    ele.push_back(indexoffset + 3);
}

void Chunk::fillRightFace(size_t x, size_t y, size_t z, BlockType type)
{
    glm::vec4 square[4] =
    {
        glm::vec4(x + cubeRadius, y, z, 1.0f),
        glm::vec4(x + cubeRadius, y + cubeRadius, z, 1.0f),
        glm::vec4(x + cubeRadius, y + cubeRadius, z + cubeRadius, 1.0f),
        glm::vec4(x + cubeRadius, y, z + cubeRadius, 1.0f)
    };
    glm::vec4 offset = glm::vec4(this->xzGlobalPos >> 32, 0.0f, this->xzGlobalPos & 0xFFFFFFFF, 1.0f);

    for (unsigned i = 0; i!=4; ++i)
    {
        square[i] += offset;
        for (unsigned j = 0; j!=4; ++j)
        {
            pos.push_back(square[i][j]);
        }
    }

    glm::vec4 normal = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    for (unsigned i = 0; i!=4; ++i)
    {
        nor.push_back(normal[i]);
    }

    switch (type)
    {
    case GRASS:
        for (unsigned i = 0; i!=4; ++i)
        {
            for (unsigned j = 0; j!=4; ++j)
            {
                col.push_back(grassColor[j]);
            }
        }
        break;
    case DIRT:
        for (unsigned i = 0; i!=4; ++i)
        {
            for (unsigned j = 0; j!=4; ++j)
            {
                col.push_back(dirtColor[j]);
            }
        }
        break;
    case STONE:
        for (unsigned i = 0; i!=4; ++i)
        {
            for (unsigned j = 0; j!=4; ++j)
            {
                col.push_back(stoneColor[j]);
            }
        }
        break;
    default:
        break;
    }

    size_t indexoffset = ele.size() / 3 * 2;
    ele.push_back(indexoffset);
    ele.push_back(indexoffset + 1);
    ele.push_back(indexoffset + 2);
    ele.push_back(indexoffset);
    ele.push_back(indexoffset + 2);
    ele.push_back(indexoffset + 3);
}

void Chunk::fillUpFace(size_t x, size_t y, size_t z, BlockType type)
{
    glm::vec4 square[4] =
    {
        glm::vec4(x, y + cubeRadius, z, 1.0f),
        glm::vec4(x, y + cubeRadius, z + cubeRadius, 1.0f),
        glm::vec4(x + cubeRadius, y + cubeRadius, z + cubeRadius, 1.0f),
        glm::vec4(x + cubeRadius, y + cubeRadius, z, 1.0f)
    };
    glm::vec4 offset = glm::vec4(this->xzGlobalPos >> 32, 0.0f, this->xzGlobalPos & 0xFFFFFFFF, 1.0f);

    for (unsigned i = 0; i!=4; ++i)
    {
        square[i] += offset;
        for (unsigned j = 0; j!=4; ++j)
        {
            pos.push_back(square[i][j]);
        }
    }

    glm::vec4 normal = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    for (unsigned i = 0; i!=4; ++i)
    {
        nor.push_back(normal[i]);
    }

    switch (type)
    {
    case GRASS:
        for (unsigned i = 0; i!=4; ++i)
        {
            for (unsigned j = 0; j!=4; ++j)
            {
                col.push_back(grassColor[j]);
            }
        }
        break;
    case DIRT:
        for (unsigned i = 0; i!=4; ++i)
        {
            for (unsigned j = 0; j!=4; ++j)
            {
                col.push_back(dirtColor[j]);
            }
        }
        break;
    case STONE:
        for (unsigned i = 0; i!=4; ++i)
        {
            for (unsigned j = 0; j!=4; ++j)
            {
                col.push_back(stoneColor[j]);
            }
        }
        break;
    default:
        break;
    }

    size_t indexoffset = ele.size() / 3 * 2;
    ele.push_back(indexoffset);
    ele.push_back(indexoffset + 1);
    ele.push_back(indexoffset + 2);
    ele.push_back(indexoffset);
    ele.push_back(indexoffset + 2);
    ele.push_back(indexoffset + 3);
}

void Chunk::fillDownFace(size_t x, size_t y, size_t z, BlockType type)
{
    glm::vec4 square[4] =
    {
        glm::vec4(x, y, z, 1.0f),
        glm::vec4(x + cubeRadius, y, z, 1.0f),
        glm::vec4(x + cubeRadius, y, z + cubeRadius, 1.0f),
        glm::vec4(x, y, z + cubeRadius, 1.0f)
    };
    glm::vec4 offset = glm::vec4(this->xzGlobalPos >> 32, 0.0f, this->xzGlobalPos & 0xFFFFFFFF, 1.0f);

    for (unsigned i = 0; i!=4; ++i)
    {
        square[i] += offset;
        for (unsigned j = 0; j!=4; ++j)
        {
            pos.push_back(square[i][j]);
        }
    }

    glm::vec4 normal = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
    for (unsigned i = 0; i!=4; ++i)
    {
        nor.push_back(normal[i]);
    }

    switch (type)
    {
    case GRASS:
        for (unsigned i = 0; i!=4; ++i)
        {
            for (unsigned j = 0; j!=4; ++j)
            {
                col.push_back(grassColor[j]);
            }
        }
        break;
    case DIRT:
        for (unsigned i = 0; i!=4; ++i)
        {
            for (unsigned j = 0; j!=4; ++j)
            {
                col.push_back(dirtColor[j]);
            }
        }
        break;
    case STONE:
        for (unsigned i = 0; i!=4; ++i)
        {
            for (unsigned j = 0; j!=4; ++j)
            {
                col.push_back(stoneColor[j]);
            }
        }
        break;
    default:
        break;
    }

    size_t indexoffset = ele.size() / 3 * 2;
    ele.push_back(indexoffset);
    ele.push_back(indexoffset + 1);
    ele.push_back(indexoffset + 2);
    ele.push_back(indexoffset);
    ele.push_back(indexoffset + 2);
    ele.push_back(indexoffset + 3);
}

void Chunk::fillFrontFace(size_t x, size_t y, size_t z, BlockType type)
{
    glm::vec4 square[4] =
    {
        glm::vec4(x, y, z + cubeRadius, 1.0f),
        glm::vec4(x + cubeRadius, y, z + cubeRadius, 1.0f),
        glm::vec4(x + cubeRadius, y + cubeRadius, z + cubeRadius, 1.0f),
        glm::vec4(x, y + cubeRadius, z + cubeRadius, 1.0f)
    };
    glm::vec4 offset = glm::vec4(this->xzGlobalPos >> 32, 0.0f, this->xzGlobalPos & 0xFFFFFFFF, 1.0f);

    for (unsigned i = 0; i!=4; ++i)
    {
        square[i] += offset;
        for (unsigned j = 0; j!=4; ++j)
        {
            pos.push_back(square[i][j]);
        }
    }

    glm::vec4 normal = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    for (unsigned i = 0; i!=4; ++i)
    {
        nor.push_back(normal[i]);
    }

    switch (type)
    {
    case GRASS:
        for (unsigned i = 0; i!=4; ++i)
        {
            for (unsigned j = 0; j!=4; ++j)
            {
                col.push_back(grassColor[j]);
            }
        }
        break;
    case DIRT:
        for (unsigned i = 0; i!=4; ++i)
        {
            for (unsigned j = 0; j!=4; ++j)
            {
                col.push_back(dirtColor[j]);
            }
        }
        break;
    case STONE:
        for (unsigned i = 0; i!=4; ++i)
        {
            for (unsigned j = 0; j!=4; ++j)
            {
                col.push_back(stoneColor[j]);
            }
        }
        break;
    default:
        break;
    }

    size_t indexoffset = ele.size() / 3 * 2;
    ele.push_back(indexoffset);
    ele.push_back(indexoffset + 1);
    ele.push_back(indexoffset + 2);
    ele.push_back(indexoffset);
    ele.push_back(indexoffset + 2);
    ele.push_back(indexoffset + 3);
}

void Chunk::fillBackFace(size_t x, size_t y, size_t z, BlockType type)
{
    glm::vec4 square[4] =
    {
        glm::vec4(x, y, z, 1.0f),
        glm::vec4(x, y + cubeRadius, z, 1.0f),
        glm::vec4(x + cubeRadius, y + cubeRadius, z, 1.0f),
        glm::vec4(x + cubeRadius, y, z, 1.0f)
    };
    glm::vec4 offset = glm::vec4(this->xzGlobalPos >> 32, 0.0f, this->xzGlobalPos & 0xFFFFFFFF, 1.0f);

    for (unsigned i = 0; i!=4; ++i)
    {
        square[i] += offset;
        for (unsigned j = 0; j!=4; ++j)
        {
            pos.push_back(square[i][j]);
        }
    }

    glm::vec4 normal = glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
    for (unsigned i = 0; i!=4; ++i)
    {
        nor.push_back(normal[i]);
    }

    switch (type)
    {
    case GRASS:
        for (unsigned i = 0; i!=4; ++i)
        {
            for (unsigned j = 0; j!=4; ++j)
            {
                col.push_back(grassColor[j]);
            }
        }
        break;
    case DIRT:
        for (unsigned i = 0; i!=4; ++i)
        {
            for (unsigned j = 0; j!=4; ++j)
            {
                col.push_back(dirtColor[j]);
            }
        }
        break;
    case STONE:
        for (unsigned i = 0; i!=4; ++i)
        {
            for (unsigned j = 0; j!=4; ++j)
            {
                col.push_back(stoneColor[j]);
            }
        }
        break;
    default:
        break;
    }

    size_t indexoffset = ele.size() / 3 * 2;
    ele.push_back(indexoffset);
    ele.push_back(indexoffset + 1);
    ele.push_back(indexoffset + 2);
    ele.push_back(indexoffset);
    ele.push_back(indexoffset + 2);
    ele.push_back(indexoffset + 3);
}

void Chunk::create()
{
    std::vector<GLfloat> unpackedPosition;
    std::vector<GLfloat> unpackedNormal;
    std::vector<GLfloat> unpackedColor;
    std::vector<GLuint> unpackedElement;

    for (size_t x = 0; x != 16; ++x)
    {
        for (size_t y = 0; y != 256; ++y)
        {
            for (size_t z = 0; z != 16; ++z)
            {
                if (this->getBlockType(x, y, z) == EMPTY)
                {
                    continue;
                }

                //Check left
                if (x == 0)
                {
                    //Check the chunk to its left
                    if (this->adjNegX == nullptr ||
                            this->adjNegX->getBlockType(15, y, z) == EMPTY)
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
                    if (this->adjPosX == nullptr ||
                            this->adjPosX->getBlockType(0, y, z) == EMPTY)
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
                    if (this->adjNegZ == nullptr ||
                            this->adjNegZ->getBlockType(x, y, 15) == EMPTY)
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
                    if (this->adjPosZ == nullptr ||
                            this->adjPosZ->getBlockType(x, y, 0) == EMPTY)
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

    this->count = unpackedElement.size();

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

    generateCol();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * col.size(),
                             reinterpret_cast<void*>(col.data()), GL_STATIC_DRAW);
}
