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
//! Wrapper around an OpenGL framebuffer object for offscreen rendering
/*! An SLGLFbo owns three GL objects: a framebuffer, a 2D colour texture bound
to GL_COLOR_ATTACHMENT0, and a renderbuffer holding a 24 bit depth buffer.
Rendering into it and then binding the result as a texture is the usual way to
implement a post-processing or reflection pass.

\warning The constructor issues OpenGL calls, so an instance may only be
created on the main thread once a GL context exists — unlike the mesh classes,
it cannot be built during asynchronous asset loading. */
class SLGLFbo
{
public:
    //! Creates the framebuffer, its colour texture and its depth renderbuffer
    /*! \param w              Width in pixels
        \param h              Height in pixels
        \param magFilter      GL magnification filter of the colour texture
        \param minFilter      GL minification filter of the colour texture
        \param internalFormat GL internal format of the colour texture
        \param format         GL data type of the colour texture pixels
        \param wrap           GL wrap mode applied to both S and T
        The previously bound framebuffer is restored before returning. A
        completeness failure is reported on stderr, not thrown. */
    SLGLFbo(SLuint w,
            SLuint h,
            SLenum magFilter      = GL_NEAREST,
            SLenum minFilter      = GL_NEAREST,
            SLint  internalFormat = GL_RGB16F,
            SLint  format         = GL_FLOAT,
            SLint  wrap           = GL_REPEAT);

    //! Deletes the texture, the renderbuffer and the framebuffer
    ~SLGLFbo();

    //! Binds the colour texture to a texture unit and sets the sampler uniform
    /*! \param progId      GL id of the shader program holding the sampler
        \param samplerName Name of the sampler uniform in that program
        \param textureUnit Texture unit to bind to, added to GL_TEXTURE0 */
    void activateAsTexture(int             progId,
                           const SLstring& samplerName,
                           int             textureUnit = 0);

    SLuint width;      //!< Width of the framebuffer in pixels
    SLuint height;     //!< Height of the framebuffer in pixels
    SLuint attachment; //!< \deprecated Never assigned or read; left uninitialised
    SLuint fboID;      //!< GL id of the framebuffer object
    SLuint texID;      //!< GL id of the colour attachment texture
    SLuint rboID;      //!< GL id of the depth renderbuffer
};
//-----------------------------------------------------------------------------
#endif // SLGLFBO_H