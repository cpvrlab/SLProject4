/**
 * \file      AppDemoSceneRTSpheres.h
 * \brief     Class declaration for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marcus Hudritsch, Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef APPDEMOSCENERTSPHERES_H
#define APPDEMOSCENERTSPHERES_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for the ray tracing scene with sphere group
class AppDemoSceneRTSpheres : public SLScene
{
public:
    AppDemoSceneRTSpheres(SLSceneID sceneID);
    void    registerAssetsToLoad(SLAssetLoader& al) override;
    void    assemble(SLAssetManager* am, SLSceneView* sv) override;
    SLNode* SphereGroupRT(SLAssetManager* am,
                          SLint           depth,
                          SLfloat         x,
                          SLfloat         y,
                          SLfloat         z,
                          SLfloat         scale,
                          SLuint          resolution,
                          SLMaterial*     matGlass,
                          SLMaterial*     matRed);
private:
    SLSceneID _sceneID;
};
//-----------------------------------------------------------------------------

#endif
