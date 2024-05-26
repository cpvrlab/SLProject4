//#############################################################################
//  File:      AppDemoScene2Dand3DText.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENE2DAND3DTEXT_H
#define APPDEMOSCENE2DAND3DTEXT_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for 2D and 3D text scene
class AppDemoScene2Dand3DText : public SLScene
{
public:
    AppDemoScene2Dand3DText();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;
};
//-----------------------------------------------------------------------------

#endif
