/**
 * \file      SLGLProgramManager.cpp
 * \date      March 2020
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 * \authors   Michael Goettlicher, Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 */

#include <SLGLProgramManager.h>
#include <SLGLProgramGeneric.h>

//-----------------------------------------------------------------------------
string                                         SLGLProgramManager::shaderPath;
string                                         SLGLProgramManager::configPath;
std::map<SLStdShaderProg, SLGLProgramGeneric*> SLGLProgramManager::_programs;
//-----------------------------------------------------------------------------
/*!
 * \param shdrPath Path to the shader files
 * \param confPath Path to the writable config directory
 */
void SLGLProgramManager::init(string shaderPath, string configPath)
{
    SLGLProgramManager::shaderPath = std::move(shaderPath);
    SLGLProgramManager::configPath = std::move(configPath);
}
//-----------------------------------------------------------------------------
void SLGLProgramManager::loadPrograms()
{
    assert(!shaderPath.empty() &&
           "Error in SLGLProgramManager: shader path is empty!");

    loadProgram(SP_colorAttribute,
                "ColorAttribute.vert",
                "Color.frag");
    loadProgram(SP_colorUniform,
                "ColorUniform.vert",
                "Color.frag");
    loadProgram(SP_textureOnly,
                "TextureOnly.vert",
                "TextureOnly.frag");
    loadProgram(SP_textureOnlyExternal,
                "TextureOnlyExternal.vert",
                "TextureOnlyExternal.frag");
    loadProgram(SP_fontTex,
                "FontTex.vert",
                "FontTex.frag");
    loadProgram(SP_errorTex,
                "ErrorTex.vert",
                "ErrorTex.frag");
    loadProgram(SP_depth,
                "Depth.vert",
                "Depth.frag");

    SLGLProgram* colorUniformPoint = loadProgram(SP_colorUniformPoint,
                                                 "ColorUniformPoint.vert",
                                                 "Color.frag");
    colorUniformPoint->addUniform1f(new SLGLUniform1f(UT_const,
                                                      "u_pointSize",
                                                      3.0f));
}
//-----------------------------------------------------------------------------
void SLGLProgramManager::deletePrograms()
{
    for (auto it : _programs)
        delete it.second;
    _programs.clear();
}
//-----------------------------------------------------------------------------
SLGLProgramGeneric* SLGLProgramManager::loadProgram(SLStdShaderProg id,
                                                    string          vertShaderFilename,
                                                    string          fragShaderFilename)
{
    SLstring vertShaderPath = shaderPath + vertShaderFilename;
    SLstring fragShaderPath = shaderPath + fragShaderFilename;

    SLGLProgramGeneric* program = new SLGLProgramGeneric(nullptr,
                                                         vertShaderPath,
                                                         fragShaderPath);
    _programs.insert({id, program});
    return program;
}
//-----------------------------------------------------------------------------
