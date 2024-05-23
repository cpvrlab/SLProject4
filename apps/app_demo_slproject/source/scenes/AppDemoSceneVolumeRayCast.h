//#############################################################################
//  File:      AppDemoSceneVolumeRayCast.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENEVOLUMERAYCAST_H
#define APPDEMOSCENEVOLUMERAYCAST_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
//! Class for test scene for volume rendering of an angiographic MRI scan"
class AppDemoSceneVolumeRayCast : public AppScene
{
public:
    AppDemoSceneVolumeRayCast();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLGLTexture* _mriTex3D;
    SLGLProgram* _sp;
};
//-----------------------------------------------------------------------------

#endif
