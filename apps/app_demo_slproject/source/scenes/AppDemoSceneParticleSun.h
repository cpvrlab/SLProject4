//#############################################################################
//  File:      AppDemoSceneParticleSun.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marcus Hudritsch, Particle System from Marc Affolter
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENEPARTICLESUN_H
#define APPDEMOSCENEPARTICLESUN_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
//! Class for sun particle system scene
class AppDemoSceneParticleSun : public AppScene
{
public:
    AppDemoSceneParticleSun();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLGLTexture* _texC;
    SLGLTexture* _texFlip;
};
//-----------------------------------------------------------------------------

#endif
