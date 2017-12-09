#ifndef SHADOWMAPFBO
#define SHADOWMAPFBO

#include <openglcontext.h>
#include <la.h>
#include <glm/glm.hpp>

#include "drawable.h"
#include "texture.h"

class ShadowMapFBO
{
public:
    ShadowMapFBO(OpenGLContext* context);

    ~ShadowMapFBO();

    bool Init(unsigned int WindowWidth, unsigned int WindowHeight);

    void BindForWriting();

    void BindForReading(GLenum TextureUnit);

private:
    OpenGLContext* context;
    GLuint m_fbo;
    GLuint m_shadowMap;
};

#endif // SHADOWMAPFBO

