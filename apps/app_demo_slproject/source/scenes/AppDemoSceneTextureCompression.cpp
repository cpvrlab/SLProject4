//#############################################################################
//  File:      AppDemoSceneTextureCompression.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneTextureCompression.h>
#include <SLLightSpot.h>
#include <SLRectangle.h>
#include <AppDemo.h>

//-----------------------------------------------------------------------------
AppDemoSceneTextureCompression::AppDemoSceneTextureCompression() : AppScene("Texture Compression Test Scene")
{
    info("Texture Compression Test Scene");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneTextureCompression::registerAssetsToLoad(SLAssetLoader& al)
{
    SLint min = GL_LINEAR_MIPMAP_LINEAR;
    SLint mag = GL_NEAREST;

    al.addTextureToLoad(_texPng,
                        AppDemo::texturePath,
                        "earth2048_C.png",
                        min,
                        mag);
    al.addTextureToLoad(_texJpgQ90,
                        AppDemo::texturePath,
                        "earth2048_C_Q90.jpg",
                        min,
                        mag);
    al.addTextureToLoad(_texJpgQ40,
                        AppDemo::texturePath,
                        "earth2048_C_Q40.jpg",
                        min,
                        mag);

    /* Console commands to generate the following KTX files
    ./../../../externals/prebuilt/mac64_ktx_v4.0.0-beta7-cpvr/release/toktx --automipmap --linear --lower_left_maps_to_s0t0 --bcmp --clevel 4 --qlevel 255 earth2048_C_bcmp_Q255.ktx2 earth2048_C.png
    ./../../../externals/prebuilt/mac64_ktx_v4.0.0-beta7-cpvr/release/toktx --automipmap --linear --lower_left_maps_to_s0t0 --bcmp --clevel 4 --qlevel 128 earth2048_C_bcmp_Q128.ktx2 earth2048_C.png
    ./../../../externals/prebuilt/mac64_ktx_v4.0.0-beta7-cpvr/release/toktx --automipmap --linear --lower_left_maps_to_s0t0 --bcmp --clevel 4 --qlevel   1 earth2048_C_bcmp_Q001.ktx2 earth2048_C.png
    ./../../../externals/prebuilt/mac64_ktx_v4.0.0-beta7-cpvr/release/toktx --automipmap --linear --lower_left_maps_to_s0t0 --uastc 4 --zcmp 19 earth2048_C_uastc4.ktx2 earth2048_C.png
    ./../../../externals/prebuilt/mac64_ktx_v4.0.0-beta7-cpvr/release/toktx --automipmap --linear --lower_left_maps_to_s0t0 --uastc 2 --zcmp 19 earth2048_C_uastc2.ktx2 earth2048_C.png
    ./../../../externals/prebuilt/mac64_ktx_v4.0.0-beta7-cpvr/release/toktx --automipmap --linear --lower_left_maps_to_s0t0 --uastc 0 --zcmp 19 earth2048_C_uastc0.ktx2 earth2048_C.png
    */

    al.addTextureToLoad(_texKtxBcmp255,
                        AppDemo::texturePath,
                        "earth2048_C_bcmp_Q255.ktx2",
                        min,
                        mag);
    al.addTextureToLoad(_texKtxBcmp128,
                        AppDemo::texturePath,
                        "earth2048_C_bcmp_Q128.ktx2",
                        min,
                        mag);
    al.addTextureToLoad(_texKtxBcmp001,
                        AppDemo::texturePath,
                        "earth2048_C_bcmp_Q001.ktx2",
                        min,
                        mag);
    al.addTextureToLoad(_texKtxUastc4,
                        AppDemo::texturePath,
                        "earth2048_C_uastc4.ktx2",
                        min,
                        mag);
    al.addTextureToLoad(_texKtxUastc2,
                        AppDemo::texturePath,
                        "earth2048_C_uastc2.ktx2",
                        min,
                        mag);
    al.addTextureToLoad(_texKtxUastc0,
                        AppDemo::texturePath,
                        "earth2048_C_uastc0.ktx2",
                        min,
                        mag);
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneTextureCompression::assemble(SLAssetManager* am, SLSceneView* sv)
{
    // Create a scene group node
    SLNode* scene = new SLNode("scene node");
    this->root3D(scene);

    // Create a light source node
    SLLightSpot* light1 = new SLLightSpot(am, this, 0.1f);
    light1->translation(5, 5, 5);
    light1->name("light node");
    scene->addChild(light1);

    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->clipNear(0.1f);
    cam1->clipFar(100);
    cam1->translation(0, 0, 4.2f);
    cam1->lookAt(0, 0, 0);
    cam1->focalDist(4.2f);
    cam1->background().colors(SLCol4f(0.7f, 0.7f, 0.7f),
                              SLCol4f(0.2f, 0.2f, 0.2f));
    cam1->setInitialState();
    scene->addChild(cam1);

    // Position for rectangle and uv out of earth texture
    SLVec2f pMin(-.5f, -.5f), pMax(.5f, .5f);
    SLVec2f tMin(.47f, .69f), tMax(.56f, .81f);

    //.........................................................................
    SLMaterial* matPng      = new SLMaterial(am,
                                        "matPng",
                                        _texPng);
    SLMesh*     rectMeshPng = new SLRectangle(am,
                                          pMin,
                                          pMax,
                                          tMin,
                                          tMax,
                                          1,
                                          1,
                                          "rectMeshPng",
                                          matPng);
    SLNode*     rectNodePng = new SLNode(rectMeshPng,
                                     "rectNodePng");
    rectNodePng->translate(-1.05f, 1.05f, 0);
    scene->addChild(rectNodePng);
    //.........................................................................
    SLMaterial* matJpgQ90      = new SLMaterial(am,
                                           "matJpgQ90",
                                           _texJpgQ90);
    SLMesh*     rectMeshJpgQ90 = new SLRectangle(am,
                                             pMin,
                                             pMax,
                                             tMin,
                                             tMax,
                                             1,
                                             1,
                                             "rectMeshJpgQ90",
                                             matJpgQ90);
    SLNode*     rectNodeJpgQ90 = new SLNode(rectMeshJpgQ90,
                                        "rectNodeJpgQ90");
    rectNodeJpgQ90->translate(0, 1.05f, 0);
    scene->addChild(rectNodeJpgQ90);
    //.........................................................................
    SLMaterial* matJpgQ40      = new SLMaterial(am,
                                           "matJpgQ40",
                                           _texJpgQ40);
    SLMesh*     rectMeshJpgQ40 = new SLRectangle(am,
                                             pMin,
                                             pMax,
                                             tMin,
                                             tMax,
                                             1,
                                             1,
                                             "rectMeshJpgQ40",
                                             matJpgQ40);
    SLNode*     rectNodeJpgQ40 = new SLNode(rectMeshJpgQ40,
                                        "rectNodeJpgQ40");
    rectNodeJpgQ40->translate(1.05f, 1.05f, 0);
    scene->addChild(rectNodeJpgQ40);
    //.........................................................................
    SLMaterial* matKtxBcmp255      = new SLMaterial(am,
                                               "matKtxBcmp255",
                                               _texKtxBcmp255);
    SLMesh*     rectMeshKtxBcmp255 = new SLRectangle(am,
                                                 pMin,
                                                 pMax,
                                                 tMin,
                                                 tMax,
                                                 1,
                                                 1,
                                                 "rectMeshKtxBcmp255",
                                                 matKtxBcmp255);
    SLNode*     rectNodeKtxBcmp255 = new SLNode(rectMeshKtxBcmp255,
                                            "rectNodeKtxBcmp255");
    rectNodeKtxBcmp255->translate(-1.05f, 0, 0);
    scene->addChild(rectNodeKtxBcmp255);
    //.........................................................................
    SLMaterial* matKtxBcmp128      = new SLMaterial(am,
                                               "matKtxBcmp128",
                                               _texKtxBcmp128);
    SLMesh*     rectMeshKtxBcmp128 = new SLRectangle(am,
                                                 pMin,
                                                 pMax,
                                                 tMin,
                                                 tMax,
                                                 1,
                                                 1,
                                                 "rectMeshKtxBcmp128",
                                                 matKtxBcmp128);
    SLNode*     rectNodeKtxBcmp128 = new SLNode(rectMeshKtxBcmp128,
                                            "rectNodeKtxBcmp128");
    rectNodeKtxBcmp128->translate(0, 0, 0);
    scene->addChild(rectNodeKtxBcmp128);
    //.........................................................................
    SLMaterial* matKtxBcmp001      = new SLMaterial(am,
                                               "matKtxBcmp001",
                                               _texKtxBcmp001);
    SLMesh*     rectMeshKtxBcmp001 = new SLRectangle(am,
                                                 pMin,
                                                 pMax,
                                                 tMin,
                                                 tMax,
                                                 1,
                                                 1,
                                                 "rectMeshKtxBcmp001",
                                                 matKtxBcmp001);
    SLNode*     rectNodeKtxBcmp001 = new SLNode(rectMeshKtxBcmp001,
                                            "rectNodeKtxBcmp001");
    rectNodeKtxBcmp001->translate(1.05f, 0, 0);
    scene->addChild(rectNodeKtxBcmp001);
    //.........................................................................
    SLMaterial* matKtxUastc4      = new SLMaterial(am,
                                              "matKtxUastc4",
                                              _texKtxUastc4);
    SLMesh*     rectMeshKtxUastc4 = new SLRectangle(am,
                                                pMin,
                                                pMax,
                                                tMin,
                                                tMax,
                                                1,
                                                1,
                                                "rectMeshKtxUastc4",
                                                matKtxUastc4);
    SLNode*     rectNodeKtxUastc4 = new SLNode(rectMeshKtxUastc4,
                                           "rectNodeKtxUastc4");
    rectNodeKtxUastc4->translate(1.05f, -1.05f, 0);
    scene->addChild(rectNodeKtxUastc4);
    //.........................................................................
    SLMaterial* matKtxUastc2      = new SLMaterial(am,
                                              "matKtxUastc2",
                                              _texKtxUastc2);
    SLMesh*     rectMeshKtxUastc2 = new SLRectangle(am,
                                                pMin,
                                                pMax,
                                                tMin,
                                                tMax,
                                                1,
                                                1,
                                                "rectMeshKtxUastc2",
                                                matKtxUastc2);
    SLNode*     rectNodeKtxUastc2 = new SLNode(rectMeshKtxUastc2,
                                           "rectNodeKtxUastc2");
    rectNodeKtxUastc2->translate(0, -1.05f, 0);
    scene->addChild(rectNodeKtxUastc2);
    //.........................................................................
    SLMaterial* matKtxUastc0      = new SLMaterial(am,
                                              "matKtxUastc0",
                                              _texKtxUastc0);
    SLMesh*     rectMeshKtxUastc0 = new SLRectangle(am,
                                                pMin,
                                                pMax,
                                                tMin,
                                                tMax,
                                                1,
                                                1,
                                                "rectMeshKtxUastc0",
                                                matKtxUastc0);
    SLNode*     rectNodeKtxUastc0 = new SLNode(rectMeshKtxUastc0,
                                           "rectNodeKtxUastc0");
    rectNodeKtxUastc0->translate(-1.05f, -1.05f, 0);
    scene->addChild(rectNodeKtxUastc0);
    //.........................................................................
    // Add active camera
    sv->camera(cam1);

    // Save energy
    sv->doWaitOnIdle(true);
}
//-----------------------------------------------------------------------------
