//#############################################################################
//  File:      AppDemoSceneShadowCascaded.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENESHADOWCASCADED_H
#define APPDEMOSCENESHADOWCASCADED_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for demo scene for cascaded shadow mapping
class AppDemoSceneShadowCascaded : public SLScene
{
public:
    AppDemoSceneShadowCascaded();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLNode* _teapot;
};
//-----------------------------------------------------------------------------

#endif
