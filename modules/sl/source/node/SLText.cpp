/**
 * \file      SLText.cpp
 * \authors   Marcus Hudritsch
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <SLScene.h>
#include <SLText.h>

//-----------------------------------------------------------------------------
/**
 * @brief Construct a new SLText::SLText object
 * @remarks It is important that during instantiation NO OpenGL functions (gl*) 
 * get called because this constructor will be most probably called in a parallel 
 * thread from within an SLScene::registerAssetsToLoad or SLScene::assemble 
 * function. All objects that get rendered have to do their OpenGL initialization 
 * when they are used the first time during rendering in the main thread.
 * @param text Text to be rendered
 * @param font SLTexTont to be used
 * @param color Color for font
 * @param maxWidth Max. line width in pixels
 * @param lineHeightFactor Line hight factor >= 1.0
 */
SLText::SLText(SLstring   text,
               SLTexFont* font,
               SLCol4f    color,
               SLfloat    maxWidth,
               SLfloat    lineHeightFactor)
  : SLNode("Text")
{
    assert(font);
    _font  = font;
    _text  = text;
    _color = color;
    _maxW  = maxWidth;
    _lineH = lineHeightFactor;
}
//-----------------------------------------------------------------------------
/*!
SLText::shapeDraw draws the text buffer objects
*/
void SLText::drawText(SLSceneView* sv)
{
    if (_drawBits.get(SL_DB_HIDDEN) || !SLGLState::instance()->blend()) return;

    // create buffer object for text once
    if (!_vao.vaoID())
    {
        _font->buildTextBuffers(_vao, _text, _maxW, _lineH);
        _font->minFiler(SL_ANISOTROPY_MAX);
        _font->magFiler(GL_LINEAR);
    }
    // Enable & build font texture with active OpenGL context
    _font->bindActive();

    // Setup shader
    SLGLProgram* sp    = _font->fontTexProgram();
    SLGLState*   state = SLGLState::instance();
    sp->useProgram();
    sp->uniformMatrix4fv("u_mMatrix", 1, (const SLfloat*)&updateAndGetWM());
    sp->uniformMatrix4fv("u_vMatrix", 1, (const SLfloat*)&state->viewMatrix);
    sp->uniformMatrix4fv("u_pMatrix", 1, (const SLfloat*)&state->projectionMatrix);
    sp->uniform4fv("u_textColor", 1, (float*)&_color);
    sp->uniform1i("u_matTexture0", 0);

    _vao.drawElementsAs(PT_triangles, (SLuint)_text.length() * 2 * 3);
}
//-----------------------------------------------------------------------------
/*!
SLText::statsRec updates the statistics.
*/
void SLText::statsRec(SLNodeStats& stats)
{
    stats.numBytes += (SLuint)sizeof(SLText);
    stats.numBytes += (SLuint)_text.length();
    stats.numNodes++;
    stats.numTriangles += (SLuint)_text.length() * 2 + 2;
}
//-----------------------------------------------------------------------------
/*!
SLText::buildAABB builds and returns the axis-aligned bounding box.
*/
SLAABBox& SLText::updateAABBRec(SLbool updateAlsoAABBinOS)
{
    SLVec2f size = _font->calcTextSize(_text);

    // calculate min & max in object space
    SLVec3f minOS(0, 0, -0.01f);
    SLVec3f maxOS(size.x, size.y, 0.01f);

    // apply world matrix: this overwrites the AABB of the group
    if (updateAlsoAABBinOS)
        _aabb.fromOStoWS(minOS, maxOS, updateAndGetWM());

    return _aabb;
}
//-----------------------------------------------------------------------------
