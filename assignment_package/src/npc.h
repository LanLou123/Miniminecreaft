#ifndef NPC_H
#define NPC_H

#include "drawable.h"
#include "la.h"
#include "scene/terrain.h"

class NPC : public Drawable
{
private:
    glm::vec3 positionWRTChunk;
    Terrain* terrain;
public:
    NPC(OpenGLContext* parent, Terrain* terrain);
    glm::mat4 getModelMatrix();
    void decisionMaking();
    virtual void create() override;
};

#endif // NPC_H
