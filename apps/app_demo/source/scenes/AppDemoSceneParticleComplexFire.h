/**
 * \file      AppDemoSceneParticleComplexFire.h
 * \brief     Class declaration for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marcus Hudritsch, Particle System from Marc Affolter
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef APPDEMOSCENEPARTICLEFIRE_H
#define APPDEMOSCENEPARTICLEFIRE_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for fire ring particle system scene
class AppDemoSceneParticleComplexFire : public SLScene
{
public:
    AppDemoSceneParticleComplexFire(SLSceneID sceneID);
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

    SLNode* createComplexFire(SLAssetManager* am,
                              SLScene*        s,
                              SLbool          withLight,
                              SLGLTexture*    texFireCld,
                              SLGLTexture*    texFireFlm,
                              SLint           flipbookCols,
                              SLint           flipbookRows,
                              SLGLTexture*    texCircle,
                              SLGLTexture*    texSmokeB,
                              SLGLTexture*    texSmokeW);
    SLNode* createTorchFire(SLAssetManager* am,
                            SLScene*        s,
                            SLbool          withLight,
                            SLGLTexture*    texFireCld,
                            SLGLTexture*    texFireFlm,
                            SLint           flipbookCols,
                            SLint           flipbookRows);

private:
    SLSceneID    _sceneID;
    SLGLTexture* _texFireCld;
    SLGLTexture* _texFireFlm;
    SLGLTexture* _texCircle;
    SLGLTexture* _texSmokeB;
    SLGLTexture* _texSmokeW;
    SLGLTexture* _texTorchFlm;
    SLGLTexture* _texTorchSmk;
    SLGLTexture* _texWallDIF;
    SLGLTexture* _texWallNRM;
    SLNode*      _firewood;
    SLNode*      _torchL;
};
//-----------------------------------------------------------------------------

#endif
