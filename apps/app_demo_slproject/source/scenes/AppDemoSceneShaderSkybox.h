/**
 * \file      AppDemoSceneShaderSkybox.h
 * \brief     Class declaration for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef APPDEMOSCENESHADERSKYBOX_H
#define APPDEMOSCENESHADERSKYBOX_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for image base lighting demo scene
class AppDemoSceneShaderSkybox : public SLScene
{
public:
    AppDemoSceneShaderSkybox();

    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLSkybox*    _skybox;
    SLGLProgram* _spRefl;
    SLGLProgram* _spRefr;
    SLNode*      _teapot;
    SLNode*      _suzanne;
};
//-----------------------------------------------------------------------------

#endif
