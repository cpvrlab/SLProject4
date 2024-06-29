/**
 * \file      AppDemoSceneJansUniverse.h
 * \brief     Class declaration for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef APPDEMOSCENEJANSUNIVERSE_H
#define APPDEMOSCENEJANSUNIVERSE_H

#include "SLGLTexture.h"
#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for the benchmark scene for Jans Universe
class AppDemoSceneJansUniverse : public SLScene
{
public:
    AppDemoSceneJansUniverse();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;
    void addUniverseLevel(SLAssetManager* am,
                          SLScene*        s,
                          SLNode*         parent,
                          SLint           parentID,
                          SLuint          currentLevel,
                          SLuint          levels,
                          SLuint          childCount,
                          SLVMaterial&    materials,
                          SLVMesh&        meshes,
                          SLuint&         numNodes);
    void generateUniverse(SLAssetManager* am,
                          SLScene*        s,
                          SLNode*         parent,
                          SLint           parentID,
                          SLuint          levels,
                          SLuint          childCount,
                          SLVMaterial&    materials,
                          SLVMesh&        meshes);

private:
    SLGLTexture* _textureC;
    SLGLTexture* _textureM;
    SLGLTexture* _textureR;
};
//-----------------------------------------------------------------------------

#endif
