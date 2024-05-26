//#############################################################################
//  File:      AppDemoSceneRevolver.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneRevolver.h>
#include <AppDemo.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>
#include <SLCone.h>
#include <SLDisk.h>
#include <SLRectangle.h>
#include <SLSphere.h>
#include <SLCylinder.h>

//-----------------------------------------------------------------------------
AppDemoSceneRevolver::AppDemoSceneRevolver() : SLScene("Revolving Mesh Test")
{
    info("Examples of revolving mesh objects constructed by rotating a 2D curve. "
         "The glass shader reflects and refracts the environment map. "
         "Try ray tracing with key R and come back with the ESC key.");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneRevolver::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addTextureToLoad(_tex1C,
                        AppDemo::texturePath +
                          "Testmap_1024_C.jpg");
    al.addTextureToLoad(_tex1N,
                        AppDemo::texturePath +
                          "Testmap_1024_N.jpg");
    al.addTextureToLoad(_tex2,
                        AppDemo::texturePath +
                          "wood0_0512_C.jpg");
    al.addTextureToLoad(_tex3,
                        AppDemo::texturePath +
                          "bricks1_0256_C.jpg");
    al.addTextureToLoad(_tex4,
                        AppDemo::texturePath +
                          "wood2_0512_C.jpg");
    al.addTextureToLoad(_tex5,
                        AppDemo::texturePath + "wood2_0256_C.jpg",
                        AppDemo::texturePath + "wood2_0256_C.jpg",
                        AppDemo::texturePath + "gray_0256_C.jpg",
                        AppDemo::texturePath + "wood0_0256_C.jpg",
                        AppDemo::texturePath + "gray_0256_C.jpg",
                        AppDemo::texturePath + "bricks1_0256_C.jpg");
    al.addProgramToLoad(_sp1,
                        AppDemo::shaderPath + "RefractReflect.vert",
                        AppDemo::shaderPath + "RefractReflect.frag");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneRevolver::assemble(SLAssetManager* am, SLSceneView* sv)
{
    // Test map material
    SLMaterial* mat1 = new SLMaterial(am, "mat1", _tex1C, _tex1N);

    // floor material
    SLMaterial* mat2 = new SLMaterial(am, "mat2", _tex2);
    mat2->specular(SLCol4f::BLACK);

    // Back wall material
    SLMaterial* mat3 = new SLMaterial(am, "mat3", _tex3);
    mat3->specular(SLCol4f::BLACK);

    // Left wall material
    SLMaterial* mat4 = new SLMaterial(am, "mat4", _tex4);
    mat4->specular(SLCol4f::BLACK);

    // Glass material
    SLMaterial* mat5 = new SLMaterial(am,
                                      "glass",
                                      SLCol4f::BLACK,
                                      SLCol4f::WHITE,
                                      255,
                                      0.1f,
                                      0.9f,
                                      1.5f);
    mat5->addTexture(_tex5);
    mat5->program(_sp1);

    // Wine material
    SLMaterial* mat6 = new SLMaterial(am,
                                      "wine",
                                      SLCol4f(0.4f, 0.0f, 0.2f),
                                      SLCol4f::BLACK,
                                      255,
                                      0.2f,
                                      0.7f,
                                      1.3f);
    mat6->addTexture(_tex5);
    mat6->program(_sp1);

    // camera
    SLCamera* cam1 = new SLCamera();
    cam1->name("cam1");
    cam1->translation(0, 1, 17);
    cam1->lookAt(0, 1, 0);
    cam1->focalDist(17);
    cam1->background().colors(SLCol4f(0.7f, 0.7f, 0.7f),
                              SLCol4f(0.2f, 0.2f, 0.2f));
    cam1->setInitialState();
    cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);

    // light
    SLLightSpot* light1 = new SLLightSpot(am,
                                          this,
                                          0,
                                          4,
                                          0,
                                          0.3f);
    light1->powers(0.2f, 1.0f, 1.0f);
    light1->attenuation(1, 0, 0);
    SLAnimation* anim = this->animManager().createNodeAnimation("light1_anim", 4.0f);
    anim->createNodeAnimTrackForEllipse(light1,
                                        6.0f,
                                        A_z,
                                        6.0f,
                                        A_x);

    // glass 2D polygon definition for revolution
    SLVVec3f revG;
    revG.push_back(SLVec3f(0.00f, 0.00f)); // foot
    revG.push_back(SLVec3f(2.00f, 0.00f));
    revG.push_back(SLVec3f(2.00f, 0.00f));
    revG.push_back(SLVec3f(2.00f, 0.10f));
    revG.push_back(SLVec3f(1.95f, 0.15f));
    revG.push_back(SLVec3f(0.40f, 0.50f)); // stand
    revG.push_back(SLVec3f(0.25f, 0.60f));
    revG.push_back(SLVec3f(0.20f, 0.70f));
    revG.push_back(SLVec3f(0.30f, 3.00f));
    revG.push_back(SLVec3f(0.30f, 3.00f)); // crack
    revG.push_back(SLVec3f(0.20f, 3.10f));
    revG.push_back(SLVec3f(0.20f, 3.10f));
    revG.push_back(SLVec3f(1.20f, 3.90f)); // outer cup
    revG.push_back(SLVec3f(1.60f, 4.30f));
    revG.push_back(SLVec3f(1.95f, 4.80f));
    revG.push_back(SLVec3f(2.15f, 5.40f));
    revG.push_back(SLVec3f(2.20f, 6.20f));
    revG.push_back(SLVec3f(2.10f, 7.10f));
    revG.push_back(SLVec3f(2.05f, 7.15f));
    revG.push_back(SLVec3f(2.00f, 7.10f)); // inner cup
    revG.push_back(SLVec3f(2.05f, 6.00f));
    SLuint  res   = 30;
    SLNode* glass = new SLNode(new SLRevolver(am,
                                              revG,
                                              SLVec3f(0, 1, 0),
                                              res,
                                              true,
                                              false,
                                              "GlassRev",
                                              mat5));
    glass->translate(0.0f, -3.5f, 0.0f, TS_object);

    // wine 2D polyline definition for revolution with two sided material
    SLVVec3f revW;
    revW.push_back(SLVec3f(0.00f, 3.82f));
    revW.push_back(SLVec3f(0.20f, 3.80f));
    revW.push_back(SLVec3f(0.80f, 4.00f));
    revW.push_back(SLVec3f(1.30f, 4.30f));
    revW.push_back(SLVec3f(1.70f, 4.80f));
    revW.push_back(SLVec3f(1.95f, 5.40f));
    revW.push_back(SLVec3f(2.05f, 6.00f));
    SLMesh* wineMesh = new SLRevolver(am, revW, SLVec3f(0, 1, 0), res, true, false, "WineRev", mat6);
    wineMesh->matOut(mat5);
    SLNode* wine = new SLNode(wineMesh);
    wine->translate(0.0f, -3.5f, 0.0f, TS_object);

    // wine fluid top
    SLNode* wineTop = new SLNode(new SLDisk(am, 2.05f, -SLVec3f::AXISY, res, false, "WineRevTop", mat6));
    wineTop->translate(0.0f, 2.5f, 0.0f, TS_object);

    // Other revolver objects
    SLNode* sphere = new SLNode(new SLSphere(am, 1, 16, 16, "sphere", mat1));
    sphere->translate(3, 0, 0, TS_object);
    SLNode* cylinder = new SLNode(new SLCylinder(am, 0.1f, 7, 3, 16, true, true, "cylinder", mat1));
    cylinder->translate(0, 0.5f, 0);
    cylinder->rotate(90, -1, 0, 0);
    cylinder->rotate(30, 0, 1, 0);
    SLNode* cone = new SLNode(new SLCone(am, 1, 3, 3, 16, true, "cone", mat1));
    cone->translate(-3, -1, 0, TS_object);
    cone->rotate(90, -1, 0, 0);

    // Cube dimensions
    SLfloat pL = -9.0f, pR = 9.0f;  // left/right
    SLfloat pB = -3.5f, pT = 14.5f; // bottom/top
    SLfloat pN = 9.0f, pF = -9.0f;  // near/far

    // bottom rectangle
    SLNode* b = new SLNode(new SLRectangle(am, SLVec2f(pL, -pN), SLVec2f(pR, -pF), 10, 10, "PolygonFloor", mat2));
    b->rotate(90, -1, 0, 0);
    b->translate(0, 0, pB, TS_object);

    // top rectangle
    SLNode* t = new SLNode(new SLRectangle(am, SLVec2f(pL, pF), SLVec2f(pR, pN), 10, 10, "top", mat2));
    t->rotate(90, 1, 0, 0);
    t->translate(0, 0, -pT, TS_object);

    // far rectangle
    SLNode* f = new SLNode(new SLRectangle(am, SLVec2f(pL, pB), SLVec2f(pR, pT), 10, 10, "far", mat3));
    f->translate(0, 0, pF, TS_object);

    // left rectangle
    SLNode* l = new SLNode(new SLRectangle(am, SLVec2f(-pN, pB), SLVec2f(-pF, pT), 10, 10, "left", mat4));
    l->rotate(90, 0, 1, 0);
    l->translate(0, 0, pL, TS_object);

    // right rectangle
    SLNode* r = new SLNode(new SLRectangle(am, SLVec2f(pF, pB), SLVec2f(pN, pT), 10, 10, "right", mat4));
    r->rotate(90, 0, -1, 0);
    r->translate(0, 0, -pR, TS_object);

    SLNode* scene = new SLNode;
    this->root3D(scene);
    scene->addChild(light1);
    scene->addChild(glass);
    scene->addChild(wine);
    scene->addChild(wineTop);
    scene->addChild(sphere);
    scene->addChild(cylinder);
    scene->addChild(cone);
    scene->addChild(b);
    scene->addChild(f);
    scene->addChild(t);
    scene->addChild(l);
    scene->addChild(r);
    scene->addChild(cam1);

    sv->camera(cam1);
}
//-----------------------------------------------------------------------------
