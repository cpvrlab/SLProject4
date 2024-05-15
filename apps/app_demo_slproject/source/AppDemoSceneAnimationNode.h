//#############################################################################
//  File:      AppDemoSceneAnimationNode.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENEANIMNODE_H
#define APPDEMOSCENEANIMNODE_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
//! Class for node animation test scene
class AppDemoSceneAnimationNode : public AppScene
{
public:
    AppDemoSceneAnimationNode();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLGLTexture* _tex1;
};
//-----------------------------------------------------------------------------

#endif
