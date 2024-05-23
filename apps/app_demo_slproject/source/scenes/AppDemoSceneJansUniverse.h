//#############################################################################
//  File:      AppDemoSceneJansUniverse.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENEJANSUNIVERSE_H
#define APPDEMOSCENEJANSUNIVERSE_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
//! Class for the benchmark scene for Jans Universe
class AppDemoSceneJansUniverse : public AppScene
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
};
//-----------------------------------------------------------------------------

#endif
