//#############################################################################
//  File:      AppDemoSceneVideoTrackFace.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENEVIDEOFACE_H
#define APPDEMOSCENEVIDEOFACE_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for facial feature tracking test scene
class AppDemoSceneVideoTrackFace : public SLScene
{
public:
    AppDemoSceneVideoTrackFace(SLSceneID sid);
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLSceneID _sceneID;
    SLNode*   _glasses;
};
//-----------------------------------------------------------------------------

#endif
