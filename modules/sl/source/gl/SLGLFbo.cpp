/**
 * \file      SLGLFbo.cpp
 * \brief     Wraps an OpenGL framebuffer object
 * \date      September 2018
 * \authors   Stefan Thoeni
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <SLGLState.h>
#include <SLGLFbo.h>

//-----------------------------------------------------------------------------
SLGLFbo::SLGLFbo(SLuint w,
                 SLuint h,
                 SLenum magFilter,
                 SLenum minFilter,
                 SLint  internalFormat,
                 SLint  format,
                 SLint  wrap) : width(w), height(h)
{
    GLint previousFrameBuffer;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFrameBuffer);

    // Init framebuffer.
    glGenFramebuffers(1, &fboID);
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);

    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    // Texture parameters.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 internalFormat,
                 w,
                 h,
                 0,
                 GL_RGBA,
                 format,
                 nullptr);

    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           texID,
                           0);

    glGenRenderbuffers(1, &rboID);
    glBindRenderbuffer(GL_RENDERBUFFER, rboID);

    // Use a single rbo for both depth and stencil buffer
    glRenderbufferStorage(GL_RENDERBUFFER,
                          GL_DEPTH_COMPONENT24,
                          w,
                          h);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                              GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER,
                              rboID);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER,
                      previousFrameBuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "FBO failed to initialize correctly." << std::endl;
}
//-----------------------------------------------------------------------------
SLGLFbo::~SLGLFbo()
{
    glDeleteTextures(1, &texID);
    glDeleteFramebuffers(1, &fboID);
}
//-----------------------------------------------------------------------------
void SLGLFbo::activateAsTexture(const int       progId,
                                const SLstring& glSamplerName,
                                const int       textureUnit)
{
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, texID);
    glUniform1i(glGetUniformLocation(progId,
                                     glSamplerName.c_str()),
                textureUnit);
}
//-----------------------------------------------------------------------------
