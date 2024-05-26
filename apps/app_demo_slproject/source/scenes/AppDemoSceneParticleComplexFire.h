//#############################################################################
//  File:      AppDemoSceneParticleComplexFire.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marcus Hudritsch, Particle System from Marc Affolter
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

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
