/**
 * \file      AppDemoSceneShaderParallax.cpp
 * \brief     Implementation for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marcus Hudritsch, Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <AppDemoSceneShaderParallax.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>
#include <SLLightDirect.h>
#include <SLRectangle.h>
#include <AppCommon.h>

//-----------------------------------------------------------------------------
AppDemoSceneShaderParallax::AppDemoSceneShaderParallax()
  : SLScene("Parallax Bump Mapping Test")
{
    info("Parallax mapping with a spot. "
         "Use F2-Key to increment (decrement w. shift) parallax scale."
         "Use F3-Key to increment (decrement w. shift) parallax offset.\n");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneShaderParallax::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addTextureToLoad(_texC,
                        AppCommon::texturePath + "brickwall0512_C.jpg");
    al.addTextureToLoad(_texN,
                        AppCommon::texturePath + "brickwall0512_N.jpg");
    al.addTextureToLoad(_texH,
                        AppCommon::texturePath + "brickwall0512_H.jpg");
    al.addProgramToLoad(_sp,
                        AppCommon::shaderPath + "PerPixBlinnTmNm.vert",
                        AppCommon::shaderPath + "PerPixBlinnTmPm.frag");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneShaderParallax::assemble(SLAssetManager* am, SLSceneView* sv)
{
    SLGLUniform1f* scale  = new SLGLUniform1f(UT_const,
                                             "u_scale",
                                             0.04f,
                                             0.002f,
                                             0,
                                             1,
                                             (SLKey)K_F2);
    SLGLUniform1f* offset = new SLGLUniform1f(UT_const,
                                              "u_offset",
                                              -0.03f,
                                              0.002f,
                                              -1,
                                              1,
                                              (SLKey)K_F3);
    this->eventHandlers().push_back(scale);
    this->eventHandlers().push_back(offset);
    _sp->addUniform1f(scale);
    _sp->addUniform1f(offset);

    // Create materials
    SLMaterial* m1 = new SLMaterial(am,
                                    "mat1",
                                    _texC,
                                    _texN,
                                    _texH,
                                    nullptr,
                                    _sp);

    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(-10, 10, 10);
    cam1->lookAt(0, 0, 0);
    cam1->focalDist(cam1->translationWS().distance(SLVec3f::ZERO));
    cam1->background().colors(SLCol4f(0.5f, 0.5f, 0.5f));
    cam1->setInitialState();

    SLLightSpot* light1 = new SLLightSpot(am,
                                          this,
                                          0.3f,
                                          40,
                                          true);
    light1->powers(0.1f, 1.0f, 1.0f);
    light1->attenuation(1, 0, 0);
    light1->translation(0, 0, 5);
    light1->lookAt(0, 0, 0);

    SLAnimation* anim = this->animManager().createNodeAnimation("light1_anim", 2.0f);
    anim->createNodeAnimTrackForEllipse(light1,
                                        2.0f,
                                        A_x,
                                        2.0f,
                                        A_y);

    SLNode* scene = new SLNode;
    this->root3D(scene);
    scene->addChild(light1);
    scene->addChild(new SLNode(new SLRectangle(am,
                                               SLVec2f(-5, -5),
                                               SLVec2f(5, 5),
                                               1,
                                               1,
                                               "Rect",
                                               m1)));
    scene->addChild(cam1);

    sv->camera(cam1);
}
//-----------------------------------------------------------------------------
