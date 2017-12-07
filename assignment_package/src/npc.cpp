#include "npc.h"

NPC::NPC(OpenGLContext *parent, Terrain *terrain) :
    Chunk(parent), terrain(terrain), positionWorld(glm::vec3(0.0f, 0.0f, 0.0f))
{}

void NPC::create()
{
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
