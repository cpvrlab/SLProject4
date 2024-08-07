/**
 * \file      SLGLVertexArrayExt.cpp
 * \brief     Extension class with functions for quick line & point drawing
 * \date      January 2016
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <SLGLState.h>
#include <SLGLProgram.h>
#include <SLGLVertexArrayExt.h>
#include <SLGLProgramManager.h>
#include <SLMaterial.h>

//-----------------------------------------------------------------------------
/*! Helper function that sets the vertex position attribute and generates or
updates the vertex buffer from it. It is used together with the
drawArrayAsColored function.
*/
void SLGLVertexArrayExt::generateVertexPos(SLuint numVertices,
                                           SLint  elementSize,
                                           void*  dataPointer)
{
    assert(dataPointer);
    assert(elementSize);
    assert(numVertices);

    SLGLProgram* sp = SLGLProgramManager::get(SP_colorUniform);
    sp->useProgram();
    SLint location = AT_position;

    // Add attribute if it doesn't exist
    if (_vbo.attribIndex(AT_position) == -1)
    {
        setAttrib(AT_position, elementSize, location, dataPointer);
        generate(numVertices, BU_static, false);
    }
    else
        updateAttrib(AT_position, elementSize, dataPointer);
}
//-----------------------------------------------------------------------------
/*! Draws the vertex positions as array with a specified primitive & color
 */
void SLGLVertexArrayExt::drawArrayAsColored(SLGLPrimitiveType primitiveType,
                                            SLCol4f           color,
                                            SLfloat           pointSize,
                                            SLuint            indexFirstVertex,
                                            SLuint            countVertices)
{
    assert(countVertices <= _numVertices);

    if (!_vbo.id())
        SL_EXIT_MSG("No VBO generated for VAO in drawArrayAsColored.");

    // Prepare shader
    SLGLProgram* sp      = SLGLProgramManager::get(SP_colorUniform);
    SLGLState*   stateGL = SLGLState::instance();
    sp->useProgram();
    sp->uniformMatrix4fv("u_mMatrix", 1, (SLfloat*)&stateGL->modelMatrix);
    sp->uniformMatrix4fv("u_vMatrix", 1, (SLfloat*)&stateGL->viewMatrix);
    sp->uniformMatrix4fv("u_pMatrix", 1, (SLfloat*)&stateGL->projectionMatrix);
    sp->uniform1f("u_oneOverGamma", 1.0f);
    stateGL->currentMaterial(nullptr);

    // Set uniform color
    glUniform4fv(sp->getUniformLocation("u_matDiff"), 1, (SLfloat*)&color);

#if not defined(SL_GLES) && not defined(SL_EMSCRIPTEN)
    if (pointSize != 1.0f)
        if (primitiveType == PT_points)
            glPointSize(pointSize);
#endif

    //////////////////////////////////////
    drawArrayAs(primitiveType,
                (SLsizei)indexFirstVertex,
                (SLsizei)countVertices);
    //////////////////////////////////////

#if not defined(SL_GLES) && not defined(SL_EMSCRIPTEN)
    if (pointSize != 1.0f)
        if (primitiveType == PT_points)
            glPointSize(1.0f);
#endif

    GET_GL_ERROR;
}
//-----------------------------------------------------------------------------
/*! Draws the vertex positions as array with a specified primitive & color
 */
void SLGLVertexArrayExt::drawElementAsColored(SLGLPrimitiveType primitiveType,
                                              SLCol4f           color,
                                              SLfloat           pointSize,
                                              SLuint            indexFirstVertex,
                                              SLuint            countVertices)
{
    assert(countVertices <= _numVertices);

    if (!_vbo.id())
        SL_EXIT_MSG("No VBO generated for VAO in drawArrayAsColored.");

    // Prepare shader
    SLGLProgram* sp      = SLGLProgramManager::get(SP_colorUniform);
    SLGLState*   stateGL = SLGLState::instance();
    sp->useProgram();
    sp->uniformMatrix4fv("u_mMatrix", 1, (SLfloat*)&stateGL->modelMatrix);
    sp->uniformMatrix4fv("u_vMatrix", 1, (SLfloat*)&stateGL->viewMatrix);
    sp->uniformMatrix4fv("u_pMatrix", 1, (SLfloat*)&stateGL->projectionMatrix);
    sp->uniform1f("u_oneOverGamma", 1.0f);
    stateGL->currentMaterial(nullptr);

    // Set uniform color
    glUniform4fv(sp->getUniformLocation("u_matDiff"), 1, (SLfloat*)&color);

#if not defined(SL_GLES) && not defined(SL_EMSCRIPTEN)
    if (pointSize != 1.0f)
        if (primitiveType == PT_points)
            glPointSize(pointSize);
#endif

    ////////////////////////////////
    drawElementsAs(primitiveType,
                   indexFirstVertex,
                   countVertices);
    ////////////////////////////////

#if not defined(SL_GLES) && not defined(SL_EMSCRIPTEN)
    if (pointSize != 1.0f)
        if (primitiveType == PT_points)
            glPointSize(1.0f);
#endif

    GET_GL_ERROR;
}
//-----------------------------------------------------------------------------
