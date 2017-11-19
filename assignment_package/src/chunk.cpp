# include <scene/terrain.h>

static const GLfloat dirtColor[4] = {0.475f, 0.333f, 0.227f, 1.0f};
static const GLfloat grassColor[4] = {0.373f, 0.624f, 0.208f, 1.0f};
static const GLfloat stoneColor[4] = {0.5f, 0.5f, 0.5f, 1.0f};

static const GLfloat cubeRadius = 1.0f;

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
    return this->blocks[4096*z + 16*y + x];
}

BlockType& Chunk::accessBlockType(size_t x, size_t y, size_t z)
{
    return this->blocks[4096*z + 16*y + x];
}

void Chunk::fillFace(glm::vec4 positions[], glm::vec4 normal, BlockType type)
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

void Chunk::fillLeftFace(size_t x, size_t y, size_t z, BlockType type)
{
    glm::vec4 square[4] =
    {
        glm::vec4(x, y, z, 1.0f),
        glm::vec4(x, y, z + cubeRadius, 1.0f),
        glm::vec4(x, y + cubeRadius, z + cubeRadius, 1.0f),
        glm::vec4(x, y + cubeRadius, z, 1.0f)
    };
    xzCoords xzCoordinate = this->getXZCoordUnpacked(this->xzGlobalPos);
    glm::vec4 offset = glm::vec4(xzCoordinate.x, 0.0f, xzCoordinate.z, 1.0f);

    for (unsigned i = 0; i!=4; ++i)
    {
        square[i] += offset;
    }

    glm::vec4 normal = glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f);
    this->fillFace(square, normal, type);
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
    xzCoords xzCoordinate = this->getXZCoordUnpacked(this->xzGlobalPos);
    glm::vec4 offset = glm::vec4(xzCoordinate.x, 0.0f, xzCoordinate.z, 1.0f);

    for (unsigned i = 0; i!=4; ++i)
    {
        square[i] += offset;
    }

    glm::vec4 normal = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
    this->fillFace(square, normal, type);
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
    xzCoords xzCoordinate = this->getXZCoordUnpacked(this->xzGlobalPos);
    glm::vec4 offset = glm::vec4(xzCoordinate.x, 0.0f, xzCoordinate.z, 1.0f);

    for (unsigned i = 0; i!=4; ++i)
    {
        square[i] += offset;
    }

    glm::vec4 normal = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
    this->fillFace(square, normal, type);
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
    xzCoords xzCoordinate = this->getXZCoordUnpacked(this->xzGlobalPos);
    glm::vec4 offset = glm::vec4(xzCoordinate.x, 0.0f, xzCoordinate.z, 1.0f);

    for (unsigned i = 0; i!=4; ++i)
    {
        square[i] += offset;
    }

    glm::vec4 normal = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
    this->fillFace(square, normal, type);
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
    xzCoords xzCoordinate = this->getXZCoordUnpacked(this->xzGlobalPos);
    glm::vec4 offset = glm::vec4(xzCoordinate.x, 0.0f, xzCoordinate.z, 1.0f);

    for (unsigned i = 0; i!=4; ++i)
    {
        square[i] += offset;
    }

    glm::vec4 normal = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    this->fillFace(square, normal, type);
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
    xzCoords xzCoordinate = this->getXZCoordUnpacked(this->xzGlobalPos);
    glm::vec4 offset = glm::vec4(xzCoordinate.x, 0.0f, xzCoordinate.z, 1.0f);

    for (unsigned i = 0; i!=4; ++i)
    {
        square[i] += offset;
    }

    glm::vec4 normal = glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
    this->fillFace(square, normal, type);
}

void Chunk::create()
{
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

    generateCol();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * col.size(),
                             reinterpret_cast<void*>(col.data()), GL_STATIC_DRAW);
}
