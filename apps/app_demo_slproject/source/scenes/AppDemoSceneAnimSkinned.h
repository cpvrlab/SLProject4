//#############################################################################
//  File:      AppDemoSceneAnimSkinned.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENEANIMSKINNED_H
#define APPDEMOSCENEANIMSKINNED_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
//! Class for skinned animation test scene
class AppDemoSceneAnimSkinned : public AppScene
{
public:
    AppDemoSceneAnimSkinned();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLNode* _char1;
    SLNode* _char2;
    SLNode* _cube1;
    SLNode* _cube2;
    SLNode* _cube3;
};
//-----------------------------------------------------------------------------

#endif
