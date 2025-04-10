/**
 * \file      AppDemoSceneShaderEarth.cpp
 * \brief     Implementation for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 * \authors   Marcus Hudritsch, Earth Shader from Markus Knecht
 * \copyright http://opensource.org/licenses/GPL-3.0
*/

#include <AppDemoSceneShaderEarth.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>
#include <SLSphere.h>
#include <AppCommon.h>

//-----------------------------------------------------------------------------
AppDemoSceneShaderEarth::AppDemoSceneShaderEarth()
  : SLScene("Earth Shader Test")
{
    info("Complex earth shader with 7 textures: day color, night color, "
         "normal, height & gloss map of earth, color & alpha-map of clouds.\n"
         "Use (SHIFT) & key F2 to change scale of the parallax mapping\n"
         "Use (SHIFT) & key F3 to change offset of the parallax mapping");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneShaderEarth::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addTextureToLoad(_texC,
                        AppCommon::texturePath +
                          "earth2048_C.png");
    al.addTextureToLoad(_texN,
                        AppCommon::texturePath +
                          "earth2048_N.jpg");
    al.addTextureToLoad(_texH,
                        AppCommon::texturePath +
                          "earth2048_H.jpg");
    al.addTextureToLoad(_texG,
                        AppCommon::texturePath +
                          "earth2048_S.jpg");
    al.addTextureToLoad(_texNC,
                        AppCommon::texturePath +
                          "earthNight2048_C.jpg");
    al.addTextureToLoad(_texClC,
                        AppCommon::texturePath +
                          "earthCloud1024_alpha_C.png");
    al.addProgramToLoad(_sp,
                        AppCommon::shaderPath + "PerPixBlinnTmNm.vert",
                        AppCommon::shaderPath + "PerPixBlinnTmNmEarth.frag");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneShaderEarth::assemble(SLAssetManager* am, SLSceneView* sv)
{
    SLGLUniform1f* scale  = new SLGLUniform1f(UT_const,
                                             "u_scale",
                                             0.02f,
                                             0.002f,
                                             0,
                                             1,
                                             (SLKey)K_F2);
    SLGLUniform1f* offset = new SLGLUniform1f(UT_const,
                                              "u_offset",
                                              -0.02f,
                                              0.002f,
                                              -1,
                                              1,
                                              (SLKey)K_F3);
    this->eventHandlers().push_back(scale);
    this->eventHandlers().push_back(offset);
    _sp->addUniform1f(scale);
    _sp->addUniform1f(offset);

    // Create materials
    SLMaterial* matEarth = new SLMaterial(am,
                                          "matEarth",
                                          _texC,
                                          _texN,
                                          _texH,
                                          _texG,
                                          _sp);
    matEarth->addTexture(_texClC);
    matEarth->addTexture(_texNC);
    matEarth->shininess(4000);
    matEarth->program(_sp);

    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(0, 0, 4);
    cam1->lookAt(0, 0, 0);
    cam1->focalDist(4);
    cam1->background().colors(SLCol4f(0, 0, 0));
    cam1->setInitialState();
    cam1->devRotLoc(&AppCommon::devRot, &AppCommon::devLoc);

    SLLightSpot* sun = new SLLightSpot(am, this);
    sun->powers(0.0f, 1.0f, 0.2f);
    sun->attenuation(1, 0, 0);

    SLAnimation* anim = this->animManager().createNodeAnimation("light1_anim",
                                                                24.0f);
    anim->createNodeAnimTrackForEllipse(sun,
                                        50.0f,
                                        A_x,
                                        50.0f,
                                        A_z);

    SLuint  res   = 30;
    SLNode* earth = new SLNode(new SLSphere(am,
                                            1,
                                            res,
                                            res,
                                            "Earth",
                                            matEarth));
    earth->rotate(90, -1, 0, 0);

    SLNode* scene = new SLNode;
    this->root3D(scene);
    scene->addChild(sun);
    scene->addChild(earth);
    scene->addChild(cam1);

    sv->camera(cam1);
}
//-----------------------------------------------------------------------------
