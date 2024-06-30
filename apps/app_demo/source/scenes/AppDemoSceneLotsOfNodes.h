/**
 * \file      AppDemoSceneLotsOfNodes.h
 * \brief     Class declaration for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef APPDEMOSCENELOTSOFNODES_H
#define APPDEMOSCENELOTSOFNODES_H

#include <SLScene.h>
#include <SLGLTexture.h>

//-----------------------------------------------------------------------------
//! Class for scene with lots of nodes
class AppDemoSceneLotsOfNodes : public SLScene
{
public:
    AppDemoSceneLotsOfNodes();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    static const int _NUM_MAT = 20;
    SLGLTexture*     _texC[_NUM_MAT];
};
//-----------------------------------------------------------------------------

#endif
