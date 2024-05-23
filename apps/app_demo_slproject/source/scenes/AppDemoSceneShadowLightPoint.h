//#############################################################################
//  File:      AppDemoSceneShadowLightPoint.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENESHADOWLIGHTPOINT_H
#define APPDEMOSCENESHADOWLIGHTPOINT_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
//! Class for demo scene for point light shadow mapping
class AppDemoSceneShadowLightPoint : public AppScene
{
public:
    AppDemoSceneShadowLightPoint();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;
};
//-----------------------------------------------------------------------------

#endif
