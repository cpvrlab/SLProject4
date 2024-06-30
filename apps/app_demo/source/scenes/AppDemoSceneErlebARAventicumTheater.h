/**
 * \file      AppDemoSceneErlebARAventicumTheater.h
 * \brief     Class declaration for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marcus Hudritsch, Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef APPDEMOSCENEERLEBAR_AVENTICUMTHT_H
#define APPDEMOSCENEERLEBAR_AVENTICUMTHT_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for ErlebAR model for Aventicum Theater.
class AppDemoSceneErlebARAventicumTheater : public SLScene
{
public:
    AppDemoSceneErlebARAventicumTheater();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLNode*      _theater;
};
//-----------------------------------------------------------------------------

#endif
