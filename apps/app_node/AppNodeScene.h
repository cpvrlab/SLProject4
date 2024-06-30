/**
 * \file      AppNodeScene.h
 * \brief     Scene creation callback for the the node demo app
 * \date      June 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef APPNODESCENE_H
#define APPNODESCENE_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
class AppNodeScene : public SLScene
{
public:
    AppNodeScene();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLNode* _axesNode;
};
//-----------------------------------------------------------------------------

#endif
