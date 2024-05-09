//#############################################################################
//  File:      AppDemoSceneMinimal.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENEMINIMAL_H
#define APPDEMOSCENEMINIMAL_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
// Class for minimal scene
class AppDemoSceneMinimal : public AppScene
{
public:
    AppDemoSceneMinimal();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLGLTexture* _texC;
};
//-----------------------------------------------------------------------------

#endif
