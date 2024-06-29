/**
 * \file      SLGLFbo.h
 * \brief     Wraps an OpenGL framebuffer object
 * \date      September 2018
 * \authors   Stefan Thoeni
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLGLFBO_H
#define SLGLFBO_H

#include <SL.h>

//-----------------------------------------------------------------------------
class SLGLFbo
{
public:
    SLGLFbo(SLuint w,
            SLuint h,
            SLenum magFilter      = GL_NEAREST,
            SLenum minFilter      = GL_NEAREST,
            SLint  internalFormat = GL_RGB16F,
            SLint  format         = GL_FLOAT,
            SLint  wrap           = GL_REPEAT);

    ~SLGLFbo();

    void activateAsTexture(int             progId,
                           const SLstring& samplerName,
                           int             textureUnit = 0);

    SLuint width;
    SLuint height;
    SLuint attachment;
    SLuint fboID;
    SLuint texID;
    SLuint rboID;
};
//-----------------------------------------------------------------------------
#endif // SLGLFBO_H