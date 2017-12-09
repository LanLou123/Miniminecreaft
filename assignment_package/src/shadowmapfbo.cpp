#include "shadowmapfbo.h"

#include <QLabel>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLFramebufferObject>


ShadowMapFBO::ShadowMapFBO(OpenGLContext *context)
    :context(context)
{
    m_fbo = 0;
    m_shadowMap = 0;
}

ShadowMapFBO::~ShadowMapFBO()
{
    if (m_fbo != 0) {
        context->glDeleteFramebuffers(1, &m_fbo);
    }

    if (m_shadowMap != 0) {
        context->glDeleteTextures(1, &m_shadowMap);
    }
}

bool ShadowMapFBO::Init(unsigned int WindowWidth, unsigned int WindowHeight)
{
    // Create the FBO
    context->glGenFramebuffers(1, &m_fbo);

    // Create the depth buffer
    context->glGenTextures(1, &m_shadowMap);
    context->glBindTexture(GL_TEXTURE_2D, m_shadowMap);
    context->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    context->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    context->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    context->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    context->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

    context->glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
   // context->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMap, 0);
    context->glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,m_shadowMap, 0);

    // Disable writes to the color buffer
    context->glDrawBuffer(GL_NONE);
    //context->glReadBuffer(GL_NONE);

    GLenum Status = context->glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
        return false;
    }

    return true;
}


void ShadowMapFBO::BindForWriting()
{
    context->glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}


void ShadowMapFBO::BindForReading(GLenum TextureUnit)
{
    context->glActiveTexture(TextureUnit);
    context->glBindTexture(GL_TEXTURE_2D, m_shadowMap);
}
