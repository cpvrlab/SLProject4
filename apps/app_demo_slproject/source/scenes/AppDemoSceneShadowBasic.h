//#############################################################################
//  File:      AppDemoSceneShadowBasic.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENESHADOWBASIC_H
#define APPDEMOSCENESHADOWBASIC_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for basic shadow mapping demo scene
class AppDemoSceneShadowBasic : public SLScene
{
public:
    AppDemoSceneShadowBasic();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;
};
//-----------------------------------------------------------------------------

#endif
