//#############################################################################
//  File:      AppDemoSceneAnimationNodeMass.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENEANIMMASS_H
#define APPDEMOSCENEANIMMASS_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
//! Class for mass animation test scene
class AppDemoSceneAnimationNodeMass : public AppScene
{
public:
    AppDemoSceneAnimationNodeMass();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
};
//-----------------------------------------------------------------------------

#endif
