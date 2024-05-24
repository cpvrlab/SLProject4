//#############################################################################
//  File:      AppDemoSceneErlebARBielBFH.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENEERLEBAR_BIELBFH_H
#define APPDEMOSCENEERLEBAR_BIELBFH_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
//! Class for ErlebAR model in Biel at BFH.
class AppDemoSceneErlebARBielBFH : public AppScene
{
public:
    AppDemoSceneErlebARBielBFH();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLNode*      _bfh;
    SLGLProgram* _spVideoBackground;
};
//-----------------------------------------------------------------------------

#endif
