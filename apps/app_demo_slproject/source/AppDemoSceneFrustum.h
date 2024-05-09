//#############################################################################
//  File:      AppDemoSceneFrustum.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENEFRUSTUM_H
#define APPDEMOSCENEFRUSTUM_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
// Class for frustum culling test scene
class AppDemoSceneFrustum : public AppScene
{
public:
    AppDemoSceneFrustum();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLGLTexture* _tex;
};
//-----------------------------------------------------------------------------

#endif
