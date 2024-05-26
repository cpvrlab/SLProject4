//#############################################################################
//  File:      AppDemoSceneLevelOfDetail.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENELEVELOFDETAIL_H
#define APPDEMOSCENELEVELOFDETAIL_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for level of detail test scene
class AppDemoSceneLevelOfDetail : public SLScene
{
public:
    AppDemoSceneLevelOfDetail(SLSceneID sceneID);
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLSceneID    _sceneID;
    SLGLTexture* _texFloorDif;
    SLGLTexture* _texFloorNrm;
    SLNode*      _columnLOD;
};
//-----------------------------------------------------------------------------

#endif
