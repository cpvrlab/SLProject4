//#############################################################################
//  File:      AppDemoSceneErlebARChristoffel.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENEERLEBAR_CHRISTOFFEL_H
#define APPDEMOSCENEERLEBAR_CHRISTOFFEL_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
//! Class for ErlebAR model Christoffel Tower in Bern.
class AppDemoSceneErlebARChristoffel : public AppScene
{
public:
    AppDemoSceneErlebARChristoffel();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLNode*      _bern;
    SLGLTexture* _cubemap;
    SLGLProgram* _spRefl;
};
//-----------------------------------------------------------------------------

#endif
