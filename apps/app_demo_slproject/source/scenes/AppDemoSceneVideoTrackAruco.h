//#############################################################################
//  File:      AppDemoSceneVideoTrackAruco.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENEVIDEOARUCO_H
#define APPDEMOSCENEVIDEOARUCO_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for ARUCO marker tracking test scene
class AppDemoSceneVideoTrackAruco : public SLScene
{
public:
    AppDemoSceneVideoTrackAruco(SLSceneID sid);
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLSceneID _sceneID;
};
//-----------------------------------------------------------------------------

#endif
