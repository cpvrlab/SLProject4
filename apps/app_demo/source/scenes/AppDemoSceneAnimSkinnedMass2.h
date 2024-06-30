/**
 * \file      AppDemoSceneAnimSkinnedMass2.h
 * \brief     Class declaration for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marcus Hudritsch, Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef APPDEMOSCENEANIMSKINNEDMASS2_H
#define APPDEMOSCENEANIMSKINNEDMASS2_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for skinned animation test scene
class AppDemoSceneAnimSkinnedMass2 : public SLScene
{
public:
    AppDemoSceneAnimSkinnedMass2();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
#ifdef SL_EMSCRIPTEN
    static const SLint _size = 10;
#else
    static const SLint _size = 20;
#endif
    SLNode*            _astroboy[_size*_size];
};
//-----------------------------------------------------------------------------

#endif
