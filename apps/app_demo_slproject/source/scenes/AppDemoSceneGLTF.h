//#############################################################################
//  File:      AppDemoSceneGLTF.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENEGLTF_H
#define APPDEMOSCENEGLTF_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for GLTF file loading demo scene
class AppDemoSceneGLTF : public SLScene
{
public:
    AppDemoSceneGLTF(SLSceneID sceneID);

    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLSceneID _sceneID;
    SLstring  _modelFile;
    SLSkybox* _skybox;
    SLNode*   _modelGLTF;
};
//-----------------------------------------------------------------------------

#endif
