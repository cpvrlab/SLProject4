/**
 * \file      AppDemoSceneShadowLightPoint.h
 * \brief     Class declaration for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef APPDEMOSCENESHADOWLIGHTPOINT_H
#define APPDEMOSCENESHADOWLIGHTPOINT_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for demo scene for point light shadow mapping
class AppDemoSceneShadowLightPoint : public SLScene
{
public:
    AppDemoSceneShadowLightPoint();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;
};
//-----------------------------------------------------------------------------

#endif
