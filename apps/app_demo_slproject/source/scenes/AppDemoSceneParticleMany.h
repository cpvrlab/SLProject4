//#############################################################################
//  File:      AppDemoSceneParticleMany.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marcus Hudritsch, Particle System from Marc Affolter
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENEPARTICLEMANY_H
#define APPDEMOSCENEPARTICLEMANY_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for benchmark scene with a particle system with many particles
class AppDemoSceneParticleMany : public SLScene
{
public:
    AppDemoSceneParticleMany();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLGLTexture* _texC;
    SLGLTexture* _texFlip;
};
//-----------------------------------------------------------------------------

#endif
