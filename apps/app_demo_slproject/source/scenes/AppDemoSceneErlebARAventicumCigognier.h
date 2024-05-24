//#############################################################################
//  File:      AppDemoSceneErlebARAventicumCigognier.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENEERLEBAR_AVENTICUMCIG_H
#define APPDEMOSCENEERLEBAR_AVENTICUMCIG_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
//! Class for ErlebAR model for Aventicum Cigognier.
class AppDemoSceneErlebARAventicumCigognier : public AppScene
{
public:
    AppDemoSceneErlebARAventicumCigognier();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLNode* _cigognier;
};
//-----------------------------------------------------------------------------

#endif
