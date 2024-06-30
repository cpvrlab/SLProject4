/**
 * \file      AppDemoSceneShaderBlinn.h
 * \brief     Class declaration for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marcus Hudritsch, Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef APPDEMOSCENESHADERBLINN_H
#define APPDEMOSCENESHADERBLINN_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for demo scene with Blinn-Phong lighting per vertex or per pixel
class AppDemoSceneShaderBlinn : public SLScene
{
public:
    AppDemoSceneShaderBlinn(SLstring name,
                            bool     perVertex);

    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    bool         _perVertex;
    SLGLProgram* _perVrtTm = nullptr;
    SLGLProgram* _perVrt = nullptr;
    SLGLProgram* _perPix = nullptr;
    SLGLTexture* _texC = nullptr;
    SLGLTexture* _texN = nullptr;
    SLGLTexture* _texH = nullptr;
};
//-----------------------------------------------------------------------------

#endif
