/**
 * \file      AppDemoScene2Dand3DText.h
 * \brief     Class declaration for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef APPDEMOSCENE2DAND3DTEXT_H
#define APPDEMOSCENE2DAND3DTEXT_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for 2D and 3D text scene
class AppDemoScene2Dand3DText : public SLScene
{
public:
    AppDemoScene2Dand3DText();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;
};
//-----------------------------------------------------------------------------

#endif
