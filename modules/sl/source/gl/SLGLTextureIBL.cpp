/**
 * \file      SLGLTextureIBL.cpp
 * \date      April 2018
 * \authors   Carlos Arauz, Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <SLAssetManager.h>
#include <SLScene.h>
#include <SLGLTextureIBL.h>

//-----------------------------------------------------------------------------
const SLint ROUGHNESS_NUM_MIP_LEVELS = 5; //! Number of mip levels for roughness cubemaps
//-----------------------------------------------------------------------------
//! ctor for generated textures from hdr textures
SLGLTextureIBL::SLGLTextureIBL(SLAssetManager* am,
                               SLstring        shaderPath,
                               SLGLTexture*    sourceTexture,
                               SLVec2i         size,
                               SLTextureType   texType,
                               SLenum          target,
                               SLbool          readBackPixels,
                               SLint           min_filter,
                               SLint           mag_filter)
{
    if (sourceTexture != nullptr)
    {
        assert(sourceTexture->texType() >= TT_hdr);
        assert(texType > TT_hdr);
    }

    _sourceTexture         = sourceTexture;
    _texType               = texType;
    _min_filter            = min_filter;
    _mag_filter            = mag_filter;
    _wrap_s                = GL_CLAMP_TO_EDGE;
    _wrap_t                = GL_CLAMP_TO_EDGE;
    _target                = target;
    _texID                 = 0;
    _bumpScale             = 1.0f;
    _resizeToPow2          = false;
    _autoCalcTM3D          = false;
    _needsUpdate           = false;
    _bytesOnGPU            = 0;
    _width                 = size.x;
    _height                = size.y;
    _bytesPerPixel         = 0;
    _deleteImageAfterBuild = false;
    _readBackPixels        = readBackPixels;

    name("Generated " + typeName());

    if (texType == TT_environmentCubemap)
        _shaderProgram = new SLGLProgramGeneric(am,
                                                shaderPath + "PBR_CubeMap.vert",
                                                shaderPath + "PBR_CylinderToCubeMap.frag");
    else if (texType == TT_irradianceCubemap)
        _shaderProgram = new SLGLProgramGeneric(am,
                                                shaderPath + "PBR_CubeMap.vert",
                                                shaderPath + "PBR_IrradianceConvolution.frag");
    else if (texType == TT_roughnessCubemap)
        _shaderProgram = new SLGLProgramGeneric(am,
                                                shaderPath + "PBR_CubeMap.vert",
                                                shaderPath + "PBR_PrefilterRoughness.frag");
    else if (texType == TT_brdfLUT)
        _shaderProgram = new SLGLProgramGeneric(am,
                                                shaderPath + "PBR_BRDFIntegration.vert",
                                                shaderPath + "PBR_BRDFIntegration.frag");

    // perspective projection with field of view of 90 degrees
    _captureProjection.perspective(90.0f, 1.0f, 0.1f, 10.0f);

    // initialize capture views: 6 views each at all 6 directions of the faces of a cube map
    // clang-format off
    SLMat4f mat;
    mat.lookAt(0, 0, 0, 1, 0, 0, 0,-1, 0); _captureViews.push_back(mat);
    mat.lookAt(0, 0, 0,-1, 0, 0, 0,-1, 0); _captureViews.push_back(mat);
    mat.lookAt(0, 0, 0, 0, 1, 0, 0, 0, 1); _captureViews.push_back(mat);
    mat.lookAt(0, 0, 0, 0,-1, 0, 0, 0,-1); _captureViews.push_back(mat);
    mat.lookAt(0, 0, 0, 0, 0, 1, 0,-1, 0); _captureViews.push_back(mat);
    mat.lookAt(0, 0, 0, 0, 0,-1, 0,-1, 0); _captureViews.push_back(mat);
    // clang-format on

    // Add pointer to the global resource vectors for deallocation
    if (am)
        am->textures().push_back(this);
}
//-----------------------------------------------------------------------------
SLGLTextureIBL::~SLGLTextureIBL()
{
    deleteData(true);

    glDeleteVertexArrays(1, &_cubeVAO);
    _cubeVAO = 0;
    glDeleteBuffers(1, &_cubeVBO);
    _cubeVBO = 0;
    glDeleteVertexArrays(1, &_quadVAO);
    _quadVAO = 0;
    glDeleteBuffers(1, &_quadVBO);
    _quadVBO = 0;
    GET_GL_ERROR;
}
//-----------------------------------------------------------------------------
/*!
 Build the texture into a cube map, rendering the texture 6 times and capturing
 each time one side of the cube (except for the BRDF LUT texture, which is
 completely generated by calculations directly with the shader).
 This is probably the most difficult OpenGL code in the project.
*/
void SLGLTextureIBL::build(SLint texUnit)
{
    // Assure to delete all images if we read the pixels back
    if (_readBackPixels)
        deleteImages();
    bool saveReadbackTextures = false;

    glGenTextures(1, &_texID);
    glBindTexture(_target, _texID);

    GLuint fboID;
    glGenFramebuffers(1, &fboID);

    GLuint rboID;
    glGenRenderbuffers(1, &rboID);

    if (_texType == TT_environmentCubemap ||
        _texType == TT_irradianceCubemap)
    {
        _bytesPerPixel  = SL_HDR_PIXEL_BYTES;
        _internalFormat = SL_HDR_GL_INTERNAL_FORMAT;

        // Build the textures for the 6 faces of a cubemap with no data
        for (SLuint i = 0; i < 6; i++)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0,
                         _internalFormat,
                         _width,
                         _height,
                         0,
                         SL_HDR_GL_FORMAT,
                         SL_HDR_GL_TYPE,
                         nullptr);
        }

        glTexParameteri(_target, GL_TEXTURE_WRAP_S, _wrap_s);
        glTexParameteri(_target, GL_TEXTURE_WRAP_T, _wrap_t);
        glTexParameteri(_target, GL_TEXTURE_WRAP_R, _wrap_t);
        glTexParameteri(_target, GL_TEXTURE_MIN_FILTER, _min_filter);
        glTexParameteri(_target, GL_TEXTURE_MAG_FILTER, _mag_filter);

        _shaderProgram->useProgram();

        if (_sourceTexture != nullptr)
            _sourceTexture->bindActive();

        _shaderProgram->uniform1i("u_environmentMap", texUnit);

        glActiveTexture(GL_TEXTURE0 + texUnit);
        glBindTexture(_sourceTexture->target(),
                      _sourceTexture->texID());
        glViewport(0, 0, _width, _height);
        glBindFramebuffer(GL_FRAMEBUFFER, fboID);
        glBindRenderbuffer(GL_RENDERBUFFER, rboID);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, _width, _height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboID);

        logFramebufferStatus();

        // Set the list of draw buffers.
        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

        for (SLuint i = 0; i < 6; i++)
        {
            SLMat4f mvp = _captureProjection * _captureViews[i];
            _shaderProgram->uniformMatrix4fv("u_mvpMatrix", 1, mvp.m());

            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                   GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                   _texID,
                                   0);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderCube();

            if (_readBackPixels)
            {
                string name = (_texType == TT_environmentCubemap)
                                ? "environmentCubemap_side"
                                : "irradianceCubemap_side";
                name += std::to_string(i) + ".png";
                readPixels(_width,
                           _height,
                           name,
                           saveReadbackTextures);
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        if (_texType == TT_environmentCubemap)
        {
            glBindTexture(GL_TEXTURE_CUBE_MAP, _texID);
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        }
    }
    else if (_texType == TT_roughnessCubemap)
    {
        assert(_sourceTexture->texType() == TT_environmentCubemap &&
               "the source texture is not an environment map");

        _bytesPerPixel  = SL_HDR_PIXEL_BYTES;
        _internalFormat = SL_HDR_GL_INTERNAL_FORMAT;

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, ROUGHNESS_NUM_MIP_LEVELS - 1);

        // Define textures for all mip levels of all six sides of the cube map
        for (SLint mipLevel = 0; mipLevel < ROUGHNESS_NUM_MIP_LEVELS; mipLevel++)
        {
            // Calculate the size of this mip level
            SLint mipWidth  = _width / (1 << mipLevel);
            SLint mipHeight = _height / (1 << mipLevel);

            for (SLint i = 0; i < 6; i++)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             mipLevel,
                             _internalFormat,
                             mipWidth,
                             mipHeight,
                             0,
                             SL_HDR_GL_FORMAT,
                             SL_HDR_GL_TYPE,
                             nullptr);
            }
        }

        _shaderProgram->useProgram();

        if (_sourceTexture != nullptr)
            _sourceTexture->bindActive();

        _shaderProgram->uniform1i("u_environmentMap", texUnit);
        _shaderProgram->uniform1f("u_environmentMapSize", (float)_sourceTexture->width());

        glActiveTexture(GL_TEXTURE0 + texUnit);
        glBindTexture(_sourceTexture->target(), _sourceTexture->texID());

        for (SLint mipLevel = 0; mipLevel < ROUGHNESS_NUM_MIP_LEVELS; ++mipLevel)
        {
            // Resize framebuffer according to mip level size
            SLint mipWidth  = _width / (1 << mipLevel);
            SLint mipHeight = _height / (1 << mipLevel);
            glViewport(0, 0, mipWidth, mipHeight);

            glBindFramebuffer(GL_FRAMEBUFFER, fboID);

            SLfloat roughness = (SLfloat)mipLevel / (SLfloat)(ROUGHNESS_NUM_MIP_LEVELS - 1);
            _shaderProgram->uniform1f("u_roughness", roughness);

            for (SLuint i = 0; i < 6; ++i)
            {
                SLMat4f mvp = _captureProjection * _captureViews[i];
                _shaderProgram->uniformMatrix4fv("u_mvpMatrix", 1, mvp.m());
                glFramebufferTexture2D(GL_FRAMEBUFFER,
                                       GL_COLOR_ATTACHMENT0,
                                       GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                       _texID,
                                       mipLevel);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                logFramebufferStatus();

                renderCube();

                if (_readBackPixels)
                {
                    string name = "roughnessCubemap_mip" +
                                  std::to_string(mipLevel) + "_side" +
                                  std::to_string(i) + ".png";
                    readPixels((SLint)mipWidth,
                               (SLint)mipHeight,
                               name,
                               saveReadbackTextures);
                }
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }
    else if (_texType == TT_brdfLUT)
    {
        _bytesPerPixel  = SL_BRDF_LUT_PIXEL_BYTES;
        _internalFormat = SL_BRDF_LUT_GL_INTERNAL_FORMAT;

        glTexImage2D(_target,
                     0,
                     _internalFormat,
                     _width,
                     _height,
                     0,
                     SL_BRDF_LUT_GL_FORMAT,
                     SL_BRDF_LUT_GL_TYPE,
                     0);
        glTexParameteri(_target, GL_TEXTURE_WRAP_S, _wrap_s);
        glTexParameteri(_target, GL_TEXTURE_WRAP_T, _wrap_t);
        glTexParameteri(_target, GL_TEXTURE_MIN_FILTER, _min_filter);
        glTexParameteri(_target, GL_TEXTURE_MAG_FILTER, _mag_filter);

        glBindFramebuffer(GL_FRAMEBUFFER, fboID);

        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D,
                               _texID,
                               0);

        glViewport(0, 0, _width, _height);
        _shaderProgram->useProgram();

        if (_sourceTexture != nullptr)
            _sourceTexture->bindActive();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        logFramebufferStatus();

        renderQuad();

        if (_readBackPixels)
        {
            string name = "brdfLUT.png";
            readPixels(_width,
                       _height,
                       name,
                       saveReadbackTextures);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glDeleteFramebuffers(1, &fboID);
    glDeleteRenderbuffers(1, &rboID);

    // Reset the viewport
    SLGLState* state = SLGLState::instance();
    auto       vp    = state->viewport();
    glViewport(vp.x, vp.y, vp.z, vp.w);
    glFinish();
    GET_GL_ERROR;
}
//-----------------------------------------------------------------------------
//! Renders 2x2 cube, used to project a texture to a cube texture with 6 sides
void SLGLTextureIBL::renderCube()
{
    // initialize (if necessary)
    if (_cubeVAO == 0)
    {
        // clang-format off
        // Define vertex positions of an inward facing cube
        // The original code from learnopengl.com used an outwards facing cube
        // with disabled face culling. With our default enabled face culling this
        // lead to a horrible error that cost us about 10kFr. of debugging.
        // Thanks to the tip of Michael Goettlicher we finally got it correct.
        float vertices[] = {
          // back face
          -1.0f, -1.0f, -1.0f, // bottom-left
           1.0f, -1.0f, -1.0f, // bottom-right
           1.0f,  1.0f, -1.0f, // top-right
           1.0f,  1.0f, -1.0f, // top-right
          -1.0f,  1.0f, -1.0f, // top-left
          -1.0f, -1.0f, -1.0f, // bottom-left
          // front face
          -1.0f, -1.0f,  1.0f, // bottom-left
           1.0f,  1.0f,  1.0f, // top-right
           1.0f, -1.0f,  1.0f, // bottom-right
           1.0f,  1.0f,  1.0f, // top-right
          -1.0f, -1.0f,  1.0f, // bottom-left
          -1.0f,  1.0f,  1.0f, // top-left
          // left face
          -1.0f,  1.0f,  1.0f, // top-right
          -1.0f, -1.0f, -1.0f, // bottom-left
          -1.0f,  1.0f, -1.0f, // top-left
          -1.0f, -1.0f, -1.0f, // bottom-left
          -1.0f,  1.0f,  1.0f, // top-right
          -1.0f, -1.0f,  1.0f, // bottom-right
          // right face
           1.0f,  1.0f,  1.0f, // top-left
           1.0f,  1.0f, -1.0f, // top-right
           1.0f, -1.0f, -1.0f, // bottom-right
           1.0f, -1.0f, -1.0f, // bottom-right
           1.0f, -1.0f,  1.0f, // bottom-left
           1.0f,  1.0f,  1.0f, // top-left
          // bottom face
          -1.0f, -1.0f, -1.0f, // top-right
           1.0f, -1.0f,  1.0f, // bottom-left
           1.0f, -1.0f, -1.0f, // top-left
           1.0f, -1.0f,  1.0f, // bottom-left
          -1.0f, -1.0f, -1.0f, // top-right
          -1.0f, -1.0f,  1.0f, // bottom-right
          // top face
           1.0f,  1.0f,  1.0f, // right-top-front
          -1.0f,  1.0f,  1.0f, // left-top-front
          -1.0f,  1.0f, -1.0f, // left-top-back
          -1.0f,  1.0f, -1.0f, // left-top-back
           1.0f,  1.0f, -1.0f, // right-top-back
           1.0f,  1.0f , 1.0f  // right-top-front
        };

        // clang-format on
        glGenVertexArrays(1, &_cubeVAO);
        glGenBuffers(1, &_cubeVBO);

        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, _cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // link vertex attributes
        glBindVertexArray(_cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    // render inwards facing cube with face culling enabled
    glBindVertexArray(_cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}
//-----------------------------------------------------------------------------
//! Renders a 2x2 XY quad, used for rendering and capturing the BRDF integral
void SLGLTextureIBL::renderQuad()
{
    if (_quadVAO == 0)
    {
        // clang-format off
        // Define a front (+Z) facing quad with texture coordinates
        float quadVertices[] = {
            // positions         // texture Coords
            -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
             1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
             1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
        };
        // clang-format on

        // setup plane VAO
        glGenVertexArrays(1, &_quadVAO);
        glGenBuffers(1, &_quadVBO);
        glBindVertexArray(_quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, _quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }

    // Render quad
    glBindVertexArray(_quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
//-----------------------------------------------------------------------------
//! Reads back the pixels into an image
/*
 * The image based lighting textures are only generated on the GPU. For
 * debugging or ray tracing we need to read back the pixels into main memory.
 */
void SLGLTextureIBL::readPixels(int width, int height, string name, bool savePNG)
{
    CVImage* image = new CVImage(width,
                                 height,
                                 SL_READ_PIXELS_CV_FORMAT,
                                 name);
    glReadPixels(0,
                 0,
                 width,
                 height,
                 SL_READ_PIXELS_GL_FORMAT,
                 GL_UNSIGNED_BYTE,
                 image->data());
    GET_GL_ERROR;

    if (savePNG)
        image->savePNG(name, 9, false);

    _images.push_back(image);
}
//-----------------------------------------------------------------------------
void SLGLTextureIBL::logFramebufferStatus()
{
#if defined(DEBUG) || defined(_DEBUG)
    GLenum fbStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch (fbStatus)
    {
        case GL_FRAMEBUFFER_COMPLETE: SL_LOG("GL_FRAMEBUFFER_COMPLETE"); break;
        case GL_FRAMEBUFFER_UNDEFINED: SL_LOG("GL_FRAMEBUFFER_UNDEFINED"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: SL_LOG("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: SL_LOG("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"); break;
        case GL_FRAMEBUFFER_UNSUPPORTED: SL_LOG("GL_FRAMEBUFFER_UNSUPPORTED"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: SL_LOG("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"); break;
        // case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: SL_LOG("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"); break;
        // case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: SL_LOG("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"); break;
        // case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: SL_LOG("GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS"); break;
        default: SL_LOG("Unknown framebuffer status!!!");
    }
#endif
}
//-----------------------------------------------------------------------------
