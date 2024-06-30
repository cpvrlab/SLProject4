/**
 * \file      AppDemoSceneGLTF.h
 * \brief     Class declaration for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marcus Hudritsch, Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef APPDEMOSCENEGLTF_H
#define APPDEMOSCENEGLTF_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for GLTF file loading demo scene
class AppDemoSceneGLTF : public SLScene
{
public:
    AppDemoSceneGLTF(SLSceneID sceneID);

    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLSceneID _sceneID;
    SLstring  _modelFile;
    SLSkybox* _skybox;
    SLNode*   _modelGLTF;
};
//-----------------------------------------------------------------------------

#endif
