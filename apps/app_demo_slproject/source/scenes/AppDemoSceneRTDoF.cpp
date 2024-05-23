//#############################################################################
//  File:      AppDemoSceneRTDoF.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneRTDoF.h>
#include <AppDemo.h>
#include <SLLightSpot.h>
#include <SLRectangle.h>
#include <SLSphere.h>

//-----------------------------------------------------------------------------
AppDemoSceneRTDoF::AppDemoSceneRTDoF()
  : AppScene("Depth of Field Ray Tracing")
{
    info("Muttenzer Box with environment mapped reflective sphere and "
         "transparent refractive glass sphere. Try ray tracing for real "
         "reflections and soft shadows.");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneRTDoF::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addTextureToLoad(_tex1,
                        AppDemo::texturePath,
                        "Checkerboard0512_C.png",
                        SL_ANISOTROPY_MAX,
                        GL_LINEAR);
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneRTDoF::assemble(SLAssetManager* am,
                                 SLSceneView*    sv)
{
    // Create root node
    SLNode* scene = new SLNode;
    root3D(scene);

    // Create textures and materials
    SLMaterial* mT = new SLMaterial(am, "mT", _tex1);
    mT->kr(0.5f);
    SLMaterial* mW = new SLMaterial(am, "mW", SLCol4f::WHITE);
    SLMaterial* mB = new SLMaterial(am, "mB", SLCol4f::GRAY);
    SLMaterial* mY = new SLMaterial(am, "mY", SLCol4f::YELLOW);
    SLMaterial* mR = new SLMaterial(am, "mR", SLCol4f::RED);
    SLMaterial* mG = new SLMaterial(am, "mG", SLCol4f::GREEN);
    SLMaterial* mM = new SLMaterial(am, "mM", SLCol4f::MAGENTA);

#ifndef SL_GLES
    SLuint numSamples = 10;
#else
    SLuint numSamples = 4;
#endif

    stringstream ss;
    ss << "Ray tracing with depth of field blur. Each pixel is sampled "
       << numSamples * numSamples
       << "x from a lens. Be patient on mobile devices.";

    info(ss.str());

    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(0, 2, 7);
    cam1->lookAt(0, 0, 0);
    cam1->focalDist(cam1->translationOS().length());
    cam1->clipFar(80);
    cam1->lensDiameter(0.4f);
    cam1->lensSamples()->samples(numSamples, numSamples);
    cam1->background().colors(SLCol4f(0.1f, 0.4f, 0.8f));
    cam1->setInitialState();
    cam1->fogIsOn(true);
    cam1->fogMode(FM_exp);
    cam1->fogDensity(0.04f);
    scene->addChild(cam1);

    SLuint  res  = 36;
    SLNode* rect = new SLNode(new SLRectangle(am,
                                              SLVec2f(-40, -10),
                                              SLVec2f(40, 70),
                                              SLVec2f(0, 0),
                                              SLVec2f(4, 4),
                                              2,
                                              2,
                                              "Rect",
                                              mT));
    rect->rotate(90, -1, 0, 0);
    rect->translate(0, 0, -0.5f, TS_object);
    scene->addChild(rect);

    SLLightSpot* light1 = new SLLightSpot(am, this, 2, 2, 0, 0.1f);
    light1->ambiDiffPowers(0.1f, 1);
    light1->attenuation(1, 0, 0);
    scene->addChild(light1);

    SLNode* balls = new SLNode;
    SLNode* sp;
    sp = new SLNode(new SLSphere(am, 0.5f, res, res, "S1", mW));
    sp->translate(2.0, 0, -4, TS_object);
    balls->addChild(sp);
    sp = new SLNode(new SLSphere(am, 0.5f, res, res, "S2", mB));
    sp->translate(1.5, 0, -3, TS_object);
    balls->addChild(sp);
    sp = new SLNode(new SLSphere(am, 0.5f, res, res, "S3", mY));
    sp->translate(1.0, 0, -2, TS_object);
    balls->addChild(sp);
    sp = new SLNode(new SLSphere(am, 0.5f, res, res, "S4", mR));
    sp->translate(0.5, 0, -1, TS_object);
    balls->addChild(sp);
    sp = new SLNode(new SLSphere(am, 0.5f, res, res, "S5", mG));
    sp->translate(0.0, 0, 0, TS_object);
    balls->addChild(sp);
    sp = new SLNode(new SLSphere(am, 0.5f, res, res, "S6", mM));
    sp->translate(-0.5, 0, 1, TS_object);
    balls->addChild(sp);
    sp = new SLNode(new SLSphere(am, 0.5f, res, res, "S7", mW));
    sp->translate(-1.0, 0, 2, TS_object);
    balls->addChild(sp);
    scene->addChild(balls);

    sv->camera(cam1);
}
//-----------------------------------------------------------------------------