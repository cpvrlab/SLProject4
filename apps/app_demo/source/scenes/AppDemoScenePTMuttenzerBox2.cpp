/**
 * \file      AppDemoScenePTMuttenzerBox2.cpp
 * \brief     Implementation for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see:
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      September 2025
 * \authors   Marcus Hudritsch, Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 */

#include <AppDemoScenePTMuttenzerBox2.h>
#include <AppCommon.h>
#include <SLAssetLoader.h>
#include <SLLightRect.h>
#include <SLRectangle.h>
#include <SLSphere.h>

//-----------------------------------------------------------------------------
AppDemoScenePTMuttenzerBox2::AppDemoScenePTMuttenzerBox2()
  : SLScene("Muttenzer Box Path Tracing with soft gloss materials")
{
    info("Muttenzer Box with a soft gloss instead of a perfect mirror sphere and "
         "a frosted red instead of a clear glass sphere: both exponents are "
         "100 and not 1000, and the transmissive color is red, so the caustic "
         "under the glass sphere is red. Only the path tracer shows this; the "
         "OpenGL preview and the ray tracer render this exactly like the "
         "original Muttenzer Box.");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoScenePTMuttenzerBox2::registerAssetsToLoad(SLAssetLoader& al)
{

    al.addTextureToLoad(_tex1,
                        AppCommon::texturePath + "MuttenzerBox+X0512_C.png",
                        AppCommon::texturePath + "MuttenzerBox-X0512_C.png",
                        AppCommon::texturePath + "MuttenzerBox+Y0512_C.png",
                        AppCommon::texturePath + "MuttenzerBox-Y0512_C.png",
                        AppCommon::texturePath + "MuttenzerBox+Z0512_C.png",
                        AppCommon::texturePath + "MuttenzerBox-Z0512_C.png");

    al.addProgramToLoad(_spRefl,
                        AppCommon::shaderPath + "Reflect.vert",
                        AppCommon::shaderPath + "Reflect.frag");
    al.addProgramToLoad(_spRefr,
                        AppCommon::shaderPath + "RefractReflect.vert",
                        AppCommon::shaderPath + "RefractReflect.frag");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoScenePTMuttenzerBox2::assemble(SLAssetManager* am,
                                           SLSceneView*    sv)
{
    SLCol4f lightEmisRGB(10.0f, 10.0f, 10.0f);
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

    // The only difference to AppDemoScenePTMuttenzerBox: both exponents are
    // 100 instead of SLMaterial::PERFECT (1000). At PERFECT the path tracer
    // takes the perfect specular resp. transmissive direction and samples no
    // lobe at all; below it the direction is drawn from the Phong lobe of that
    // exponent, which is wider the smaller the exponent is. So the mirror
    // sphere gets a blurred reflection and the glass sphere both a blurred
    // refraction and, since point 19, a blurred surface reflection.
    const SLfloat glossiness = 100.0f;

    // Material for the glossy mirror sphere
    SLMaterial* refl = new SLMaterial(am,
                                      "refl",
                                      blackRGB,
                                      SLCol4f::WHITE,
                                      glossiness,
                                      1.0f);
    refl->addTexture(_tex1);
    refl->program(_spRefl);

    // Material for the frosted glass sphere. The specular color has to stay
    // black, otherwise SLRay::hitMatIsReflective would match first and the
    // transmissive branch would never run. Both exponents get the same value,
    // because a rough dielectric is rough on both sides of the interface: the
    // translucency widens the transmitted lobe and the shininess the Fresnel
    // reflected one. With the shininess left at PERFECT the sphere would still
    // mirror the light rectangle as a razor sharp quad while its transmission
    // is frosted.
    SLMaterial* refr = new SLMaterial(am,
                                      "refr",
                                      blackRGB,
                                      blackRGB,
                                      glossiness,
                                      0.05f,
                                      0.95f,
                                      1.5f);
    refr->translucency(glossiness);

    // The transmissive color is what the path tracer multiplies the light
    // passing through the sphere by, so a red one turns the glass into a red
    // filter and its caustic on the floor red with it. Green and blue are not
    // set to zero: a channel at 0 is opaque to that channel, and the caustic
    // would then be a pure red with no shading left in it at all.
    //
    // Note that the same color also tints the Fresnel reflection at the
    // surface, because SLPathtracer::trace uses the transmissive color as the
    // object color of the whole transmissive branch. For a dielectric that is
    // wrong -- the reflection off glass is uncolored, the color comes from the
    // absorption on the way through -- so the mirrored image of the light
    // rectangle on this sphere will come out red as well. See the note in
    // point 19 of docs/ImplementationPlan.md.
    refr->transmissive(SLCol4f(1.0f, 0.2f, 0.2f));
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
    cam1->devRotLoc(&AppCommon::devRot, &AppCommon::devLoc);

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
