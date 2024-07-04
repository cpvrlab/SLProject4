/**
 * \file      SLGLProgramManager.h
 * \date      March 2020
 * \authors   Michael Goettlicher, Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLGLPROGRAM_MANAGER_H
#define SLGLPROGRAM_MANAGER_H

#include <string>
#include <map>

using std::string;

class SLGLProgram;
class SLGLProgramGeneric;

//-----------------------------------------------------------------------------
//! Enumeration for standard shader programs
enum SLStdShaderProg
{
    SP_colorAttribute = 0,
    SP_colorUniform,
    SP_colorUniformPoint,
    SP_textureOnly,
    SP_textureOnlyExternal,
    SP_fontTex,
    SP_depth,
    SP_errorTex,
};

//-----------------------------------------------------------------------------
//! Static container for standard shader programs
/*!
 * Static container for standard shader programs that are not deleted after
 * scene deallocation. The shader program allocation and compilation will be
 * done at the first use. ONLY shader programs that are scene independent
 * should be stored here. Shader programs that depend e.g. on the number of
 * lights must be created at scene loading time and deallocation at scene
 * destruction.
 */
class SLGLProgramManager
{
public:
    //! Init by providing path to standard shader files
    static void init(string shaderPath, string configPath);

    //! Get program reference for given id
    static SLGLProgramGeneric* get(SLStdShaderProg id) { return _programs[id]; }

    //! Instantiate and load all programs
    static void loadPrograms();

    //! Delete all instantiated programs
    static void deletePrograms();

    //! Returns the size of the program map
    static size_t size() { return _programs.size(); }

    //! Contains the global shader path
    static string shaderPath;

    //! Contains the global writable configuration path
    static string configPath;

private:
    //! Make a new program and insert it into _programs
    static SLGLProgramGeneric* loadProgram(SLStdShaderProg id,
                                           string          vertShaderFilename,
                                           string          fragShaderFilename);

    //! Instantiated programs
    static std::map<SLStdShaderProg, SLGLProgramGeneric*> _programs;
};
//-----------------------------------------------------------------------------

#endif
