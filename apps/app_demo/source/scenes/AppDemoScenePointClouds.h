/**
 * \file      AppDemoScenePointClouds.h
 * \brief     Class declaration for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marcus Hudritsch, Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef APPDEMOSCENEPOINTCLOUDS_H
#define APPDEMOSCENEPOINTCLOUDS_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for point cloud test scene
class AppDemoScenePointClouds : public SLScene
{
public:
    AppDemoScenePointClouds();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLGLProgram* _sp1;
    SLGLProgram* _sp2;
};
//-----------------------------------------------------------------------------

#endif
