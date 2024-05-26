//#############################################################################
//  File:      AppDemoSceneZFighting.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENEZFIGHTING_H
#define APPDEMOSCENEZFIGHTING_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for z-fighting test scene
class AppDemoSceneZFighting : public SLScene
{
public:
    AppDemoSceneZFighting();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;
};
//-----------------------------------------------------------------------------

#endif
