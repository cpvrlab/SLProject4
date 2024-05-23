//#############################################################################
//  File:      AppDemoSceneVideoTrackChessboard.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENEVIDEOTRACKCHESS_H
#define APPDEMOSCENEVIDEOTRACKCHESS_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
//! Class for video for chessboard test scene
class AppDemoSceneVideoTrackChessboard : public AppScene
{
public:
    AppDemoSceneVideoTrackChessboard(SLSceneID sid);
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLSceneID _sceneID;
};
//-----------------------------------------------------------------------------

#endif
