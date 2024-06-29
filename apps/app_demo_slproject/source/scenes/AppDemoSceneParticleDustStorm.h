/**
 * \file      AppDemoSceneParticleDustStorm.h
 * \brief     Class declaration for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 * \authors   Marcus Hudritsch, Particle System from Marc Affolter
 * \copyright http://opensource.org/licenses/GPL-3.0
*/

#ifndef APPDEMOSCENEPARTICLEDUST_H
#define APPDEMOSCENEPARTICLEDUST_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for simple particle system scene
class AppDemoSceneParticleDustStorm : public SLScene
{
public:
    AppDemoSceneParticleDustStorm();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLGLTexture* _texC;
    SLGLTexture* _texFlip;
};
//-----------------------------------------------------------------------------

#endif
