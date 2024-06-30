/**
 * \file      AppDemoSceneRevolver.h
 * \brief     Class declaration for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef APPDEMOSCENEREVOLVER_H
#define APPDEMOSCENEREVOLVER_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for revolver mesh scene
class AppDemoSceneRevolver : public SLScene
{
public:
    AppDemoSceneRevolver();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLGLTexture* _tex1C;
    SLGLTexture* _tex1N;
    SLGLTexture* _tex2;
    SLGLTexture* _tex3;
    SLGLTexture* _tex4;
    SLGLTexture* _tex5;
    SLGLProgram* _sp1;
};
//-----------------------------------------------------------------------------

#endif
