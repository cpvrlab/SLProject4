/**
 * \file      SLGLVertexBuffer.cpp
 * \brief     Wrapper around an OpenGL Vertex Array Objects
 * \date      January 2016
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <SLGLState.h>
#include <SLGLVertexBuffer.h>

//-----------------------------------------------------------------------------
SLuint SLGLVertexBuffer::totalBufferSize  = 0;
SLuint SLGLVertexBuffer::totalBufferCount = 0;
//-----------------------------------------------------------------------------
//! Constructor initializing with default values
SLGLVertexBuffer::SLGLVertexBuffer()
{
    _id                  = 0;
    _numVertices         = 0;
    _sizeBytes           = 0;
    _outputIsInterleaved = false;
    _usage               = BU_stream;
}
//-----------------------------------------------------------------------------
/*! Deletes the OpenGL objects for the vertex array and the vertex buffer.
The vector _attribs with the attribute information is not cleared.
*/
void SLGLVertexBuffer::deleteGL()
{
    if (_id)
    {
        glDeleteBuffers(1, &_id);
        _id = 0;
        totalBufferCount--;
        totalBufferSize -= _sizeBytes;
    }
}
//-----------------------------------------------------------------------------
void SLGLVertexBuffer::clear()
{
    deleteGL();
    _attribs.clear();
}
//-----------------------------------------------------------------------------
SLint SLGLVertexBuffer::attribIndex(SLGLAttributeType type)
{
    for (SLuint i = 0; i < _attribs.size(); ++i)
        if (_attribs[i].type == type)
            return (SLint)i;
    return -1;
}
//-----------------------------------------------------------------------------
/*! Updates the specified vertex attribute. This works only for sequential
attributes and not for interleaved attributes. This is used e.g. for meshes
with vertex skinning. See SLMesh::draw where we have joint attributes.
*/
void SLGLVertexBuffer::updateAttrib(SLGLAttributeType type,
                                    SLint             elementSize,
                                    void*             dataPointer)
{
    assert(dataPointer && "No data pointer passed");
    assert(elementSize > 0 && elementSize < 5 && "Element size invalid");

    // Get attribute index and check element size
    SLint index = attribIndex(type);
    if (index == -1)
        SL_EXIT_MSG("Attribute type does not exist in VBO.");
    if (_attribs[(SLuint)index].elementSize != elementSize)
        SL_EXIT_MSG("Attribute element size differs.");
    if (_outputIsInterleaved)
        SL_EXIT_MSG("Interleaved buffers can't be updated.");

    // Generate the vertex buffer object if there is none
    if (index && !_id)
        glGenBuffers(1, &_id);

    _attribs[(SLuint)index].dataPointer = dataPointer;

    ///////////////////////////////////////////////
    // copy sub-data into existing buffer object //
    ///////////////////////////////////////////////

    glBindBuffer(GL_ARRAY_BUFFER, _id);
    glBufferSubData(GL_ARRAY_BUFFER,
                    _attribs[(SLuint)index].offsetBytes,
                    _attribs[(SLuint)index].bufferSizeBytes,
                    _attribs[(SLuint)index].dataPointer);
    GET_GL_ERROR;
}
//-----------------------------------------------------------------------------
/*! Generates the OpenGL VBO for one or more vertex attributes.
If the input data is an interleaved array (all attribute data pointer where
identical) also the output buffer will be generated as an interleaved array.
Vertex arrays with attributes that are updated can not be interleaved.
Vertex attributes with separate arrays can generate an interleaved or a
sequential vertex buffer.\n\n
<PRE>
\n Sequential attribute layout:
\n           |          Positions          |           Normals           |     TexCoords     |
\n Attribs:  |   Position0  |   Position1  |    Normal0   |    Normal1   |TexCoord0|TexCoord1|
\n Elements: | PX | PY | PZ | PX | PY | PZ | NX | NY | NZ | NX | NY | NZ | TX | TY | TX | TY |
\n Bytes:    |#### #### ####|#### #### ####|#### #### ####|#### #### ####|#### ####|#### ####|
\n           |                             |                             |
\n           |<------ offset Normals ----->|                             |
\n           |<-------------------- offset TexCoords ------------------->|
\n
\n Interleaved attribute layout:
\n           |               Vertex 0                |               Vertex 1                |
\n Attribs:  |   Position0  |    Normal0   |TexCoord0|   Position1  |    Normal1   |TexCoord1|
\n Elements: | PX | PY | PZ | NX | NY | NZ | TX | TY | PX | PY | PZ | NX | NY | NZ | TX | TY |
\n Bytes:    |#### #### ####|#### #### ####|#### ####|#### #### ####|#### #### ####|#### ####|
\n           |              |              |         |
\n           |<-offsetN=32->|              |         |
\n           |<------- offsetTC=32 ------->|         |
\n           |                                       |
\n           |<---------- strideBytes=32 ----------->|
</PRE>
*/
void SLGLVertexBuffer::generate(SLuint          numVertices,
                                SLGLBufferUsage usage,
                                SLbool          outputInterleaved)
{
    assert(numVertices);

    // if buffers exist delete them first
    deleteGL();

    _numVertices         = numVertices;
    _usage               = usage;
    _outputIsInterleaved = outputInterleaved;

    // Generate the vertex buffer object
    if (_attribs.size())
    {
        glGenBuffers(1, &_id);
        glBindBuffer(GL_ARRAY_BUFFER, _id);
    }

    // Check first if all attribute data pointer point to the same interleaved data
    _inputIsInterleaved = false;
    if (_attribs.size() > 1)
    {
        _inputIsInterleaved = true;
        for (auto a : _attribs)
        {
            if (a.dataPointer != _attribs[0].dataPointer)
            {
                _inputIsInterleaved = false;
                break;
            }
        }
    }

    ///////////////////////////////////////////////////////
    // Calculate total VBO size & attribute stride & offset
    ///////////////////////////////////////////////////////

    _sizeBytes   = 0;
    _strideBytes = 0;

    if (_inputIsInterleaved)
    {
        _outputIsInterleaved = true;

        for (SLuint i = 0; i < _attribs.size(); ++i)
        {
            SLuint elementSizeBytes = (SLuint)_attribs[i].elementSize *
                                      sizeOfType(_attribs[i].dataType);
            _attribs[i].offsetBytes     = _strideBytes;
            _attribs[i].bufferSizeBytes = elementSizeBytes * _numVertices;
            _sizeBytes += _attribs[i].bufferSizeBytes;
            _strideBytes += elementSizeBytes;
        }
    }
    else // input is in separate attribute data blocks
    {
        for (SLuint i = 0; i < _attribs.size(); ++i)
        {
            SLuint elementSizeBytes = (SLuint)_attribs[i].elementSize *
                                      sizeOfType(_attribs[i].dataType);
            if (_outputIsInterleaved)
                _attribs[i].offsetBytes = _strideBytes;
            else
                _attribs[i].offsetBytes = _sizeBytes;
            _attribs[i].bufferSizeBytes = elementSizeBytes * _numVertices;
            _sizeBytes += _attribs[i].bufferSizeBytes;
            if (_outputIsInterleaved) _strideBytes += elementSizeBytes;
        }
    }

    //////////////////////////////
    // Generate VBO for Attributes
    //////////////////////////////

    if (_inputIsInterleaved)
    {
        // generate the interleaved VBO buffer on the GPU
        glBufferData(GL_ARRAY_BUFFER,
                     _sizeBytes,
                     _attribs[0].dataPointer,
                     _usage);
    }
    else // input is in separate attribute data block
    {
        if (_outputIsInterleaved) // Copy attribute data interleaved
        {
            SLVuchar data;
            data.resize(_sizeBytes);
            for (auto a : _attribs)
            {
                SLuint elementSizeBytes = (SLuint)a.elementSize *
                                          sizeOfType(a.dataType);

                // Copy attributes interleaved
                for (SLuint v = 0; v < _numVertices; ++v)
                {
                    SLuint iDst = v * _strideBytes + a.offsetBytes;
                    SLuint iSrc = v * elementSizeBytes;
                    for (SLuint b = 0; b < elementSizeBytes; ++b)
                        data[iDst + b] = ((SLuchar*)a.dataPointer)[iSrc + b];
                }

                // generate the interleaved VBO buffer on the GPU
                glBufferData(GL_ARRAY_BUFFER,
                             _sizeBytes,
                             &data[0],
                             _usage);
            }
        }
        else // copy attributes buffers sequentially
        {
            // allocate the VBO buffer on the GPU
            glBufferData(GL_ARRAY_BUFFER,
                         _sizeBytes,
                         nullptr,
                         _usage);

            for (auto a : _attribs)
            {
                if (a.location > -1)
                {
                    // Copies the attributes data at the right offset into the VBO
                    glBufferSubData(GL_ARRAY_BUFFER,
                                    a.offsetBytes,
                                    a.bufferSizeBytes,
                                    a.dataPointer);
                }
            }
        }
    }

    totalBufferCount++;
    totalBufferSize += _sizeBytes;
    GET_GL_ERROR;
}

//-----------------------------------------------------------------------------
/*! This method is only used by SLGLVertexArray drawing methods for OpenGL
 * contexts prior to 3.0 where vertex array objects did not exist. This is the
 * additional overhead that had to be done per draw call.
 * For VAO (Vertex Array Objects) this method is only used once at the creation
 * of the VAO. The instanceDivisor number is only used for instanced drawing.
 * For all other usage it is by default 0.
 */
void SLGLVertexBuffer::bindAndEnableAttrib(SLuint instanceDivisor) const
{
    /////////////////////////////////////////
    // Associate VBO to Attribute location //
    /////////////////////////////////////////

    glBindBuffer(GL_ARRAY_BUFFER, _id);

    if (_outputIsInterleaved) // Copy attribute data interleaved
    {
        for (auto a : _attribs)
        {
            if (a.location > -1)
            { // Sets the vertex attribute data pointer to its corresponding GLSL variable
                if (a.dataType == BT_int || a.dataType == BT_uint)
                {
                    glVertexAttribIPointer((SLuint)a.location,
                                           a.elementSize,
                                           a.dataType,
                                           (SLint)_strideBytes,
                                           (void*)(size_t)a.offsetBytes);
                }
                else
                {
                    glVertexAttribPointer((SLuint)a.location,
                                          a.elementSize,
                                          a.dataType,
                                          GL_FALSE,
                                          (SLint)_strideBytes,
                                          (void*)(size_t)a.offsetBytes);
                }

                // Tell the attribute to be an array attribute instead of a state variable
                glEnableVertexAttribArray((SLuint)a.location);

                // Special setting for instanced drawing
                if (instanceDivisor > 0)
                    glVertexAttribDivisor((SLuint)a.location,
                                          instanceDivisor);
            }
        }
    }
    else // copy attributes buffers sequentially
    {
        for (auto a : _attribs)
        {
            if (a.location > -1)
            {
                // Sets the vertex attribute data pointer to its corresponding GLSL variable
                if (a.dataType == BT_int || a.dataType == BT_uint)
                {
                    glVertexAttribIPointer((SLuint)a.location,
                                           a.elementSize,
                                           a.dataType,
                                           0,
                                           (void*)(size_t)a.offsetBytes);
                }
                else
                {
                    glVertexAttribPointer((SLuint)a.location,
                                          a.elementSize,
                                          a.dataType,
                                          GL_FALSE,
                                          0,
                                          (void*)(size_t)a.offsetBytes);
                }

                // Tell the attribute to be an array attribute instead of a state variable
                glEnableVertexAttribArray((SLuint)a.location);

                // Special setting for instanced drawing
                if (instanceDivisor > 0)
                    glVertexAttribDivisor((SLuint)a.location,
                                          instanceDivisor);
            }
        }
    }
}
//-----------------------------------------------------------------------------
/*! This method is only used by SLGLVertexArray drawing methods for OpenGL
contexts prior to 3.0 where vertex array objects did not exist. This is the
additional overhead that had to be done per draw call.
*/
void SLGLVertexBuffer::disableAttrib()
{
    if (_attribs.size())
    {
        for (auto a : _attribs)
            if (a.location > -1)
                glDisableVertexAttribArray((SLuint)a.location);
    }
}
//-----------------------------------------------------------------------------
//! Returns the size in byte depending off the buffer type
SLuint SLGLVertexBuffer::sizeOfType(SLGLBufferType type)
{
    switch (type)
    {
        case BT_float: return sizeof(SLfloat);
        case BT_int: return sizeof(SLint);
        case BT_ubyte: return sizeof(SLuchar);
        case BT_ushort: return sizeof(SLushort);
        case BT_uint: return sizeof(SLint);
        default: SL_EXIT_MSG("Invalid buffer data type");
    }
    return 0;
}
//-----------------------------------------------------------------------------
