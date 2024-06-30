/**
 * \file      AppDemoSceneAnimSkinned.h
 * \brief     Class declaration for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marcus Hudritsch, Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef APPDEMOSCENEANIMSKINNED_H
#define APPDEMOSCENEANIMSKINNED_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for skinned animation test scene
class AppDemoSceneAnimSkinned : public SLScene
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
