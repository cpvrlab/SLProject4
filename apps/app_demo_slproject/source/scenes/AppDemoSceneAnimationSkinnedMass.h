//#############################################################################
//  File:      AppDemoSceneAnimationSkinnedMass.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENEANIMSKINNEDMASS_H
#define APPDEMOSCENEANIMSKINNEDMASS_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
//! Class for skinned animation test scene
class AppDemoSceneAnimationSkinnedMass : public AppScene
{
public:
    AppDemoSceneAnimationSkinnedMass();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLNode* _center;
};
//-----------------------------------------------------------------------------

#endif
