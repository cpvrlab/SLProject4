/**
 * \file      AppDemoSceneRTLens.cpp
 * \brief     Implementation for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <AppDemoSceneRTLens.h>
#include <AppDemo.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>
#include <SLRectangle.h>
#include <SLLens.h>

//-----------------------------------------------------------------------------
AppDemoSceneRTLens::AppDemoSceneRTLens()
  : SLScene("Ray tracing through a lens")
{
    info("Ray tracing through a lens");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneRTLens::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addTextureToLoad(_tex1,
                        AppDemo::texturePath +
                          "VisionExample.jpg");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneRTLens::assemble(SLAssetManager* am,
                                  SLSceneView*    sv)
{
    // Create root node
    SLNode* scene = new SLNode;
    root3D(scene);

    // Create textures and materials
    SLMaterial* mT = new SLMaterial(am,
                                    "mT",
                                    _tex1,
                                    nullptr,
                                    nullptr,
                                    nullptr);
    mT->kr(0.5f);

    // Glass material
    SLMaterial* matLens = new SLMaterial(am,
                                         "lens",
                                         SLCol4f(0.0f, 0.0f, 0.0f),
                                         SLCol4f(0.5f, 0.5f, 0.5f),
                                         100,
                                         0.5f,
                                         0.5f,
                                         1.5f);
    // SLGLShaderProg* sp1 = new SLGLShaderProgGeneric("RefractReflect.vert", "RefractReflect.frag");
    // matLens->shaderProg(sp1);

#ifndef APP_USES_GLES
    SLuint numSamples = 10;
#else
    SLuint numSamples = 6;
#endif

    // Scene
    SLCamera* cam1 = new SLCamera;
    cam1->translation(0, 8, 0);
    cam1->lookAt(0, 0, 0);
    cam1->focalDist(6);
    cam1->lensDiameter(0.4f);
    cam1->lensSamples()->samples(numSamples, numSamples);
    cam1->background().colors(SLCol4f(0.1f, 0.4f, 0.8f));
    cam1->setInitialState();
    cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);
    scene->addChild(cam1);

    SLLightSpot* light1 = new SLLightSpot(am, this, 1, 6, 1, 0.1f);
    light1->attenuation(0, 0, 1);
    scene->addChild(light1);

    SLuint  res  = 20;
    SLNode* rect = new SLNode(new SLRectangle(am,
                                              SLVec2f(-5, -5),
                                              SLVec2f(5, 5),
                                              res,
                                              res,
                                              "Rect",
                                              mT));
    rect->rotate(90, -1, 0, 0);
    rect->translate(0, 0, -0.0f, TS_object);
    scene->addChild(rect);

    // Lens from eye prescription card
    // SLNode* lensA = new SLNode(new SLLens(s, 0.50f, -0.50f, 4.0f, 0.0f, 32, 32, "presbyopic", matLens));   // Weitsichtig
    // lensA->translate(-2, 1, -2);
    // scene->addChild(lensA);

    // SLNode* lensB = new SLNode(new SLLens(s, -0.65f, -0.10f, 4.0f, 0.0f, 32, 32, "myopic", matLens));      // Kurzsichtig
    // lensB->translate(2, 1, -2);
    // scene->addChild(lensB);

    // Lens with radius
    // SLNode* lensC = new SLNode(new SLLens(s, 5.0, 4.0, 4.0f, 0.0f, 32, 32, "presbyopic", matLens));        // Weitsichtig
    // lensC->translate(-2, 1, 2);
    // scene->addChild(lensC);

    SLNode* lensD = new SLNode(new SLLens(am,
                                          -15.0f,
                                          -15.0f,
                                          1.0f,
                                          0.1f,
                                          res,
                                          res,
                                          "myopic",
                                          matLens)); // Kurzsichtig
    lensD->translate(0, 6, 0);
    scene->addChild(lensD);

    sv->camera(cam1);
}
//-----------------------------------------------------------------------------