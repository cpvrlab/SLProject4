//#############################################################################
//  File:      AppDemoSceneShadowLightSpot.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENESHADOWLIGHTSPOT_H
#define APPDEMOSCENESHADOWLIGHTSPOT_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for demo scene for spotlight shadow mapping
class AppDemoSceneShadowLightSpot : public SLScene
{
public:
    AppDemoSceneShadowLightSpot();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;
};
//-----------------------------------------------------------------------------

#endif
