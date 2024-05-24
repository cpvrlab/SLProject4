//#############################################################################
//  File:      AppDemoSceneShadowLightTypes.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENESHADOWLIGHTTYPES_H
#define APPDEMOSCENESHADOWLIGHTTYPES_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
//! Class for demo scene with all shadow mapping types
class AppDemoSceneShadowLightTypes : public AppScene
{
public:
    AppDemoSceneShadowLightTypes();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLNode* _teapot;
};
//-----------------------------------------------------------------------------

#endif