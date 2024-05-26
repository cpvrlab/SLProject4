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

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for ErlebAR model in Biel at BFH.
class AppDemoSceneErlebARBielBFH : public SLScene
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
