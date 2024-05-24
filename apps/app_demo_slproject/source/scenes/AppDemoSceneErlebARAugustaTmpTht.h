//#############################################################################
//  File:      AppDemoSceneErlebARAugustaTmp.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENEERLEBAR_AUGUSTATMPTHT_H
#define APPDEMOSCENEERLEBAR_AUGUSTATMPTHT_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
//! Class for ErlebAR model for Augusta Raurica Tempel & Theater.
class AppDemoSceneErlebARAugustaTmpTht : public AppScene
{
public:
    AppDemoSceneErlebARAugustaTmpTht();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLNode*      _thtAndTmp;
    SLGLTexture* _cubemap;
    SLGLProgram* _spRefl;
};
//-----------------------------------------------------------------------------

#endif
