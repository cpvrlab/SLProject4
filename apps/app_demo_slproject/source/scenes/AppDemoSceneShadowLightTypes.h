/**
 * \file      AppDemoSceneShadowLightTypes.h
 * \brief     Class declaration for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef APPDEMOSCENESHADOWLIGHTTYPES_H
#define APPDEMOSCENESHADOWLIGHTTYPES_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for demo scene with all shadow mapping types
class AppDemoSceneShadowLightTypes : public SLScene
{
public:
    AppDemoSceneShadowLightTypes();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLNode* _teapot;
};
//-----------------------------------------------------------------------------

#endif
