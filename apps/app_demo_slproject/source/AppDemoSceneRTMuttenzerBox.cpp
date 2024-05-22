//#############################################################################
//  File:      AppDemoSceneRTMuttenzerBox.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneRTMuttenzerBox.h>
#include <AppDemo.h>
#include <SLLightRect.h>
#include <SLRectangle.h>
#include <SLSphere.h>

//-----------------------------------------------------------------------------
AppDemoSceneRTMuttenzerBox::AppDemoSceneRTMuttenzerBox()
  : AppScene("Muttenzer Box Ray Tracing")
{
    info("Muttenzer Box with environment mapped reflective sphere and "
         "transparent refractive glass sphere. Try ray tracing for real "
         "reflections and soft shadows.");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneRTMuttenzerBox::registerAssetsToLoad(SLAssetLoader& al)
{

    al.addTextureToLoad(_tex1,
                        "MuttenzerBox+X0512_C.png",
                        "MuttenzerBox-X0512_C.png",
                        "MuttenzerBox+Y0512_C.png",
                        "MuttenzerBox-Y0512_C.png",
                        "MuttenzerBox+Z0512_C.png",
                        "MuttenzerBox-Z0512_C.png");

    al.addProgramGenericToLoad(_spRefl,
                               "Reflect.vert",
                               "Reflect.frag");
    al.addProgramGenericToLoad(_spRefr,
                               "RefractReflect.vert",
                               "RefractReflect.frag");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneRTMuttenzerBox::assemble(SLAssetManager* am,
                                          SLSceneView* sv)
{
    SLCol4f lightEmisRGB(7.0f, 7.0f, 7.0f);
    SLCol4f grayRGB(0.75f, 0.75f, 0.75f);
    SLCol4f redRGB(0.75f, 0.25f, 0.25f);
    SLCol4f blueRGB(0.25f, 0.25f, 0.75f);
    SLCol4f blackRGB(0.00f, 0.00f, 0.00f);

    // create materials
    SLMaterial* cream = new SLMaterial(am,
                                       "cream",
                                       grayRGB,
                                       SLCol4f::BLACK,
                                       0);
    SLMaterial* red   = new SLMaterial(am,
                                     "red",
                                     redRGB,
                                     SLCol4f::BLACK,
                                     0);
    SLMaterial* blue  = new SLMaterial(am,
                                      "blue",
                                      blueRGB,
                                      SLCol4f::BLACK,
                                      0);

    // Material for mirror sphere
    SLMaterial* refl = new SLMaterial(am,
                                      "refl",
                                      blackRGB,
                                      SLCol4f::WHITE,
                                      1000,
                                      1.0f);
    refl->addTexture(_tex1);
    refl->program(_spRefl);

    // Material for glass sphere
    SLMaterial* refr = new SLMaterial(am,
                                      "refr",
                                      blackRGB,
                                      blackRGB,
                                      100,
                                      0.05f,
                                      0.95f,
                                      1.5f);
    refr->translucency(1000);
    refr->transmissive(SLCol4f::WHITE);
    refr->addTexture(_tex1);
    refr->program(_spRefr);

    SLNode* sphere1 = new SLNode(new SLSphere(am,
                                              0.5f,
                                              32,
                                              32,
                                              "Sphere1",
                                              refl));
    sphere1->translate(-0.65f, -0.75f, -0.55f, TS_object);

    SLNode* sphere2 = new SLNode(new SLSphere(am,
                                              0.45f,
                                              32,
                                              32,
                                              "Sphere2",
                                              refr));
    sphere2->translate(0.73f, -0.8f, 0.10f, TS_object);

    SLNode* balls = new SLNode;
    balls->addChild(sphere1);
    balls->addChild(sphere2);

    // Rectangular light
    SLLightRect* lightRect = new SLLightRect(am,
                                             this,
                                             1,
                                             0.65f);
    lightRect->rotate(90, -1.0f, 0.0f, 0.0f);
    lightRect->translate(0.0f, -0.25f, 1.18f, TS_object);
    lightRect->spotCutOffDEG(90);
    lightRect->spotExponent(1.0);
    lightRect->ambientColor(SLCol4f::WHITE);
    lightRect->ambientPower(0.25f);
    lightRect->diffuseColor(lightEmisRGB);
    lightRect->attenuation(0, 0, 1);
    lightRect->samplesXY(11, 7);
    lightRect->createsShadows(true);
    lightRect->createShadowMap();

    SLLight::globalAmbient.set(lightEmisRGB * 0.01f);

    // create camera
    SLCamera* cam1 = new SLCamera();
    cam1->translation(0, 0, 7.2f);
    cam1->fov(27);
    cam1->focalDist(cam1->translationOS().length());
    cam1->background().colors(SLCol4f(0.0f, 0.0f, 0.0f));
    cam1->setInitialState();
    cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);

    // assemble scene
    SLNode* scene = new SLNode;
    root3D(scene);
    scene->addChild(cam1);
    scene->addChild(lightRect);

    // create wall polygons
    SLfloat pL = -1.48f, pR = 1.48f; // left/right
    SLfloat pB = -1.25f, pT = 1.19f; // bottom/top
    SLfloat pN = 1.79f, pF = -1.55f; // near/far

    // bottom plane
    SLNode* b = new SLNode(new SLRectangle(am,
                                           SLVec2f(pL, -pN),
                                           SLVec2f(pR, -pF),
                                           6,
                                           6,
                                           "bottom",
                                           cream));
    b->rotate(90, -1, 0, 0);
    b->translate(0, 0, pB, TS_object);
    scene->addChild(b);

    // top plane
    SLNode* t = new SLNode(new SLRectangle(am,
                                           SLVec2f(pL, pF),
                                           SLVec2f(pR, pN),
                                           6,
                                           6,
                                           "top",
                                           cream));
    t->rotate(90, 1, 0, 0);
    t->translate(0, 0, -pT, TS_object);
    scene->addChild(t);

    // far plane
    SLNode* f = new SLNode(new SLRectangle(am,
                                           SLVec2f(pL, pB),
                                           SLVec2f(pR, pT),
                                           6,
                                           6,
                                           "far",
                                           cream));
    f->translate(0, 0, pF, TS_object);
    scene->addChild(f);

    // left plane
    SLNode* l = new SLNode(new SLRectangle(am,
                                           SLVec2f(-pN, pB),
                                           SLVec2f(-pF, pT),
                                           6,
                                           6,
                                           "left",
                                           red));
    l->rotate(90, 0, 1, 0);
    l->translate(0, 0, pL, TS_object);
    scene->addChild(l);

    // right plane
    SLNode* r = new SLNode(new SLRectangle(am,
                                           SLVec2f(pF, pB),
                                           SLVec2f(pN, pT),
                                           6,
                                           6,
                                           "right",
                                           blue));
    r->rotate(90, 0, -1, 0);
    r->translate(0, 0, -pR, TS_object);
    scene->addChild(r);

    scene->addChild(balls);

    sv->camera(cam1);
}
//-----------------------------------------------------------------------------