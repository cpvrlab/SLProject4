/**
 * \file      AppDemoSceneAnimNodeMass2.h
 * \brief     Class declaration for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marcus Hudritsch, Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef APPDEMOSCENEANIMMASS2_H
#define APPDEMOSCENEANIMMASS2_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for benchmark mass animation test scene
class AppDemoSceneAnimNodeMass2 : public SLScene
{
public:
    AppDemoSceneAnimNodeMass2();
    void    registerAssetsToLoad(SLAssetLoader& al) override;
    void    assemble(SLAssetManager* am, SLSceneView* sv) override;
    SLNode* RotatingSpheres(SLAssetManager* am,
                                SLScene*        s,
                                SLint           depth,
                                SLfloat         x,
                                SLfloat         y,
                                SLfloat         z,
                                SLfloat         scale,
                                SLuint          resolution,
                                SLVMaterial&    mat);
};
//-----------------------------------------------------------------------------

#endif
