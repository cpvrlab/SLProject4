//#############################################################################
//  File:      AppDemoSceneAnimSkinnedMass2.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENEANIMSKINNEDMASS2_H
#define APPDEMOSCENEANIMSKINNEDMASS2_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
//! Class for skinned animation test scene
class AppDemoSceneAnimSkinnedMass2 : public AppScene
{
public:
    AppDemoSceneAnimSkinnedMass2();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    static const SLint _size = 20;
    SLNode*            _astroboy[_size*_size];
};
//-----------------------------------------------------------------------------

#endif
