//#############################################################################
//  File:      AppDemoSceneParticleDustStorm.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marcus Hudritsch, Particle System from Marc Affolter
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

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
