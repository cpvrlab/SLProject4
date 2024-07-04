/**
 * \file      SLGLVertexArrayExt.h
 * \brief     Extension class with functions for quick line & point drawing
 * \date      January 2016
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLGLVERTEXARRAYEXT_H
#define SLGLVERTEXARRAYEXT_H

#include <SLGLVertexArray.h>

//-----------------------------------------------------------------------------
//! SLGLVertexArray adds Helper Functions for quick Line & Point Drawing
/*! These functions where separated from SLGLVertexAttribute because they use
the predefined shader ColorUniform and have there for references to the classes
SLScene and SLGLProgram.
*/
class SLGLVertexArrayExt : public SLGLVertexArray
{
public:
    SLGLVertexArrayExt() { ; }
    virtual ~SLGLVertexArrayExt() { ; }

    //! Adds or updates & generates a position vertex attribute for colored line or point drawing
    void generateVertexPos(SLVVec2f* p) { generateVertexPos((SLuint)p->size(), 2, &p->operator[](0)); }

    //! Adds or updates & generates a position vertex attribute for colored line or point drawing
    void generateVertexPos(SLVVec3f* p) { generateVertexPos((SLuint)p->size(), 3, &p->operator[](0)); }

    //! Adds or updates & generates a position vertex attribute for colored line or point drawing
    void generateVertexPos(SLVVec4f* p) { generateVertexPos((SLuint)p->size(), 4, &p->operator[](0)); }

    //! Draws the array as the specified primitive with the color
    void drawArrayAsColored(SLGLPrimitiveType primitiveType,
                            SLCol4f           color,
                            SLfloat           lineOrPointSize  = 1.0f,
                            SLuint            indexFirstVertex = 0,
                            SLuint            countVertices    = 0);

    //! Draws the VAO by element indices with the specified primitive with the color
    void drawElementAsColored(SLGLPrimitiveType primitiveType,
                              SLCol4f           color,
                              SLfloat           pointSize,
                              SLuint            indexFirstVertex = 0,
                              SLuint            countVertices    = 0);

private:
    //! Adds or updates & generates a position vertex attribute for colored line or point drawing
    void generateVertexPos(SLuint numVertices,
                           SLint  elementSize,
                           void*  dataPointer);
};
//-----------------------------------------------------------------------------

#endif
