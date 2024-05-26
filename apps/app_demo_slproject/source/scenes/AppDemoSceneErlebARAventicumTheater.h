//#############################################################################
//  File:      AppDemoSceneErlebARAventicumTheater.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENEERLEBAR_AVENTICUMTHT_H
#define APPDEMOSCENEERLEBAR_AVENTICUMTHT_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for ErlebAR model for Aventicum Theater.
class AppDemoSceneErlebARAventicumTheater : public SLScene
{
public:
    AppDemoSceneErlebARAventicumTheater();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLNode*      _theater;
};
//-----------------------------------------------------------------------------

#endif
