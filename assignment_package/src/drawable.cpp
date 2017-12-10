#include "drawable.h"
#include <la.h>

Drawable::Drawable(OpenGLContext* context)
    : bufIdx(), bufPos(), bufNor(), bufCol(), bufUV(), bufFlowVelocity(),
      idxBound(false), posBound(false), norBound(false), colBound(false),
      tangentBound(false), bitangentBound(false), blockTypeBound(false),
      context(context), isCreated(false)
{}

Drawable::~Drawable()
{}


void Drawable::destroy()
{
    context->glDeleteBuffers(1, &bufIdx);
    context->glDeleteBuffers(1, &bufPos);
    context->glDeleteBuffers(1, &bufNor);
    context->glDeleteBuffers(1, &bufCol);
    context->glDeleteBuffers(1, &bufUV);
    context->glDeleteBuffers(1, &bufFlowVelocity);
    context->glDeleteBuffers(1, &bufTangent);
    context->glDeleteBuffers(1, &bufBiTangent);
    context->glDeleteBuffers(1, &bufBlockType);

    this->isCreated = false;
}

GLenum Drawable::drawMode()
{
    // Since we want every three indices in bufIdx to be
    // read to draw our Drawable, we tell that the draw mode
    // of this Drawable is GL_TRIANGLES

    // If we wanted to draw a wireframe, we would return GL_LINES

    return GL_TRIANGLES;
}

int Drawable::elemCount()
{
    return count;
}

int Drawable::elemCountF()
{
    return countF;
}

void Drawable::generateIdx()
{
    idxBound = true;
    // Create a VBO on our GPU and store its handle in bufIdx
    context->glGenBuffers(1, &bufIdx);
}

void Drawable::generatePos()
{
    posBound = true;
    // Create a VBO on our GPU and store its handle in bufPos
    context->glGenBuffers(1, &bufPos);
}

void Drawable::generateNor()
{
    norBound = true;
    // Create a VBO on our GPU and store its handle in bufNor
    context->glGenBuffers(1, &bufNor);
}

void Drawable::generateCol()
{
    colBound = true;
    // Create a VBO on our GPU and store its handle in bufCol
    context->glGenBuffers(1, &bufCol);
}

void Drawable::generateUV()
{
    uvBound = true;
    context->glGenBuffers(1, &bufUV);
}

void Drawable::generateFlowVelocity()
{
    flowVelocityBound = true;
    context->glGenBuffers(1, &bufFlowVelocity);
}

void Drawable::generateTangent()
{
    tangentBound = true;
    context->glGenBuffers(1, &bufTangent);
}

void Drawable::generateBiTangent()
{
    bitangentBound = true;
    context->glGenBuffers(1, &bufBiTangent);
}

void Drawable::generateBlockType()
{
    blockTypeBound = true;
    context->glGenBuffers(1, &bufBlockType);
}



void Drawable::generateIdxF()
{
    idxBoundF = true;
    // Create a VBO on our GPU and store its handle in bufIdx
    context->glGenBuffers(1, &bufIdxF);
}

void Drawable::generatePosF()
{
    posBoundF = true;
    // Create a VBO on our GPU and store its handle in bufPos
    context->glGenBuffers(1, &bufPosF);
}

void Drawable::generateNorF()
{
    norBoundF = true;
    // Create a VBO on our GPU and store its handle in bufNor
    context->glGenBuffers(1, &bufNorF);
}

void Drawable::generateColF()
{
    colBoundF = true;
    // Create a VBO on our GPU and store its handle in bufCol
    context->glGenBuffers(1, &bufColF);
}

void Drawable::generateUVF()
{
    uvBoundF = true;
    context->glGenBuffers(1, &bufUVF);
}

void Drawable::generateFlowVelocityF()
{
    flowVelocityBoundF = true;
    context->glGenBuffers(1, &bufFlowVelocityF);
}

void Drawable::generateTangentF()
{
    tangentBoundF = true;
    context->glGenBuffers(1, &bufTangentF);
}

void Drawable::generateBiTangentF()
{
    bitangentBoundF = true;
    context->glGenBuffers(1, &bufBiTangentF);
}

void Drawable::generateBlockTypeF()
{
    blockTypeBoundF = true;
    context->glGenBuffers(1, &bufBlockTypeF);
}


bool Drawable::bindIdx()
{
    if(idxBound) {
        context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    }
    return idxBound;
}

bool Drawable::bindPos()
{
    if(posBound){
        context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    }
    return posBound;
}

bool Drawable::bindNor()
{
    if(norBound){
        context->glBindBuffer(GL_ARRAY_BUFFER, bufNor);
    }
    return norBound;
}

bool Drawable::bindCol()
{
    if(colBound){
        context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    }
    return colBound;
}

bool Drawable::bindUV()
{
    if(uvBound){
        context->glBindBuffer(GL_ARRAY_BUFFER, bufUV);
    }
    return uvBound;
}

bool Drawable::bindFlowVelocity()
{
    if(flowVelocityBound){
        context->glBindBuffer(GL_ARRAY_BUFFER, bufFlowVelocity);
    }
    return flowVelocityBound;
}

bool Drawable::bindTangent()
{
    if(tangentBound){
        context->glBindBuffer(GL_ARRAY_BUFFER, bufTangent);
    }
    return tangentBound;
}

bool Drawable::bindBiTangent()
{
    if(bitangentBound){
        context->glBindBuffer(GL_ARRAY_BUFFER, bufBiTangent);
    }
    return bitangentBound;
}

bool Drawable::bindBlockType()
{
    if(blockTypeBound){
        context->glBindBuffer(GL_ARRAY_BUFFER, bufBlockType);
    }
    return blockTypeBound;
}



bool Drawable::bindIdxF()
{
    if(idxBoundF) {
        context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdxF);
    }
    return idxBoundF;
}

bool Drawable::bindPosF()
{
    if(posBoundF){
        context->glBindBuffer(GL_ARRAY_BUFFER, bufPosF);
    }
    return posBoundF;
}

bool Drawable::bindNorF()
{
    if(norBoundF){
        context->glBindBuffer(GL_ARRAY_BUFFER, bufNorF);
    }
    return norBoundF;
}

bool Drawable::bindColF()
{
    if(colBoundF){
        context->glBindBuffer(GL_ARRAY_BUFFER, bufColF);
    }
    return colBoundF;
}

bool Drawable::bindUVF()
{
    if(uvBoundF){
        context->glBindBuffer(GL_ARRAY_BUFFER, bufUVF);
    }
    return uvBoundF;
}

bool Drawable::bindFlowVelocityF()
{
    if(flowVelocityBoundF){
        context->glBindBuffer(GL_ARRAY_BUFFER, bufFlowVelocityF);
    }
    return flowVelocityBoundF;
}

bool Drawable::bindTangentF()
{
    if(tangentBoundF){
        context->glBindBuffer(GL_ARRAY_BUFFER, bufTangentF);
    }
    return tangentBoundF;
}

bool Drawable::bindBiTangentF()
{
    if(bitangentBoundF){
        context->glBindBuffer(GL_ARRAY_BUFFER, bufBiTangentF);
    }
    return bitangentBoundF;
}

bool Drawable::bindBlockTypeF()
{
    if(blockTypeBoundF){
        context->glBindBuffer(GL_ARRAY_BUFFER, bufBlockTypeF);
    }
    return blockTypeBoundF;
}
