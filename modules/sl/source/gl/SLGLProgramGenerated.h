/**
 * \file      SLGLProgramGenerated.h
 * \authors   Marcus Hudritsch
 * \details   Defines a generated shader program that just starts and stops the
 *            shaders that are hold in the base class SLGLProgram.
 * \date      December 2020
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLGLPROGRAMMGENERATED_H
#define SLGLPROGRAMMGENERATED_H

#include <SLGLProgram.h>

class SLMaterial;
class SLAssetManager;

//-----------------------------------------------------------------------------
//! Generated Shader Program class inherited from SLGLProgram
/*! An instance of this class generates the shader code on the fly at
 construction time based on the information of the passed material and lights
 vector. The generated program depends on the following parameters:
 - mat->reflectionModel (Blinn-Phong or Cook-Torrance)
 - mat->textures
 - light->createsShadows
 - active camera for the fog and projection parameters

 The shader program gets a unique name with the following pattern:
<pre>
 genCook-D00-N00-E00-O01-RM00-Sky-C4s
    |    |   |   |   |   |    |   |
    |    |   |   |   |   |    |   + Directional light w. 4 shadow cascades
    |    |   |   |   |   |    + Ambient light from skybox
    |    |   |   |   |   + Roughness-metallic map with index 0 and uv 0
    |    |   |   |   + Ambient Occlusion map with index 0 and uv 1
    |    |   |   + Emissive Map with index 0 and uv 0
    |    |   + Normal Map with index 0 and uv 0
    |    + Diffuse Texture Mapping with index 0 and uv 0
    + Cook-Torrance or Blinn-Phong lighting model
</pre>
 The above example is for a material with 5 textures and a scene with one light.
 The shader program is constructed when a material is for the first time
 activated (SLMaterial::activate) and it's program pointer is null. The old
 system of custom written GLSL shader program is still valid.
 At the end of SLMaterial::activate the generated vertex and fragment shader
 get compiled, linked and activated with the OpenGL functions in SLGLShader
 and SLGLProgram.
 After successful compilation the shader get exported into the applications
 config directory if they not yet exist there.
*/
class SLGLProgramGenerated : public SLGLProgram
{
public:
    ~SLGLProgramGenerated() override = default;

    //! ctor for generated shader program
    SLGLProgramGenerated(SLAssetManager* am,
                         const string&   programName,
                         SLMaterial*     mat,
                         SLVLight*       lights,
                         SLbool          supportGPUSkinning)
      : SLGLProgram(am,
                    "",
                    "",
                    "",
                    programName)
    {
        buildProgramCode(mat, lights, supportGPUSkinning);
    }

    //! ctor for generated shader program PS
    SLGLProgramGenerated(SLAssetManager* am,
                         const string&   programName,
                         SLMaterial*     mat,
                         bool            isDrawProg,
                         SLstring        geomShader = "")
      : SLGLProgram(am,
                    "",
                    "",
                    geomShader,
                    programName)
    {

        if (geomShader != "")
        {
            buildProgramCodePS(mat, isDrawProg, false);
        }
        else
        {
            buildProgramCodePS(mat, isDrawProg, true);
        }
    }

    static bool lightsDoShadowMapping(SLVLight* lights);
    static void buildProgramName(SLMaterial* mat,
                                 SLVLight*   lights,
                                 SLbool      supportGPUSkinning,
                                 string&     programName);
    static void buildProgramNamePS(SLMaterial* mat,
                                   string&     programName,
                                   bool        isDrawProg,
                                   bool        drawInstanced);

    void buildProgramCodePS(SLMaterial* mat,
                            bool        isDrawProg,
                            bool        drawInstanced = false);
    void buildProgramCode(SLMaterial* mat,
                          SLVLight*   lights,
                          SLbool      supportGPUSkinning);
    void beginShader(SLCamera*   cam,
                     SLMaterial* mat,
                     SLVLight*   lights) override { beginUse(cam, mat, lights); }
    void endShader() override { endUse(); }

private:
    void buildPerPixCook(SLMaterial* mat, SLVLight* lights, SLbool supportGPUSkinning);
    void buildPerPixBlinn(SLMaterial* mat, SLVLight* lights, SLbool supportGPUSkinning);
    void buildPerPixParticle(SLMaterial* mat);
    void buildPerPixParticleInstanced(SLMaterial* mat);
    void buildPerPixParticleUpdate(SLMaterial* mat);

    // Video background shader builder functions
    void buildPerPixVideoBkgdSm(SLVLight* lights);

    // Helpers
    static string fragInput_u_lightSm(SLVLight* lights);
    static string fragInput_u_shadowMaps(SLVLight* lights);
    static string fragFunctionShadowTest(SLVLight* lights);
    static string shaderHeader(int numLights);
    static string shaderHeader();
    static void   addCodeToShader(SLGLShader*   shader,
                                  const string& code,
                                  const string& name);
    static void   setVariable(std::string&       code,
                              const std::string& name,
                              const std::string& value);

    static string generatedShaderPath; //! Path to write out generated shaders
};
//-----------------------------------------------------------------------------
#endif
