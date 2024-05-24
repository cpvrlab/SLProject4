//#############################################################################
//  File:      AppDemoSceneErlebARSutz.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENEERLEBAR_SUTZ_H
#define APPDEMOSCENEERLEBAR_SUTZ_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
//! Class for ErlebAR model in Sutz
class AppDemoSceneErlebARSutz : public AppScene
{
public:
    AppDemoSceneErlebARSutz();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLNode*      _sutz;
    SLGLProgram* _spVideoBackground;
};
//-----------------------------------------------------------------------------

#endif
