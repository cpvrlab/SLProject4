//#############################################################################
//  File:      AppDemoSceneTextureFilter.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneTextureFilter.h>
#include <SLLightSpot.h>
#include <SLPolygon.h>
#include <SLSphere.h>
#include <AppDemo.h>

//-----------------------------------------------------------------------------
AppDemoSceneTextureFilter::AppDemoSceneTextureFilter() : AppScene("Texture Filter Test")
{
    info("Texture minification filters: "
         "Bottom: nearest, left: linear, top: linear mipmap, right: anisotropic. "
         "The center sphere uses a 3D texture with linear filtering.");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneTextureFilter::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addTextureToLoad(_texB,
                        AppDemo::texturePath,
                        "brick0512_C.png",
                        GL_LINEAR,
                        GL_LINEAR);
    al.addTextureToLoad(_texL,
                        AppDemo::texturePath,
                        "brick0512_C.png",
                        GL_NEAREST,
                        GL_NEAREST);
    al.addTextureToLoad(_texT,
                        AppDemo::texturePath,
                        "brick0512_C.png",
                        GL_LINEAR_MIPMAP_LINEAR,
                        GL_LINEAR);
    al.addTextureToLoad(_texR,
                        AppDemo::texturePath,
                        "brick0512_C.png",
                        SL_ANISOTROPY_MAX,
                        GL_LINEAR);
    al.addTextureToLoad(_tex3D,
                        256,
                        AppDemo::texturePath,
                        "Wave_radial10_256C.jpg");
    al.addProgramGenericToLoad(_spr3D,
                               "TextureOnly3D.vert",
                               "TextureOnly3D.frag");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneTextureFilter::assemble(SLAssetManager* am, SLSceneView* sv)
{
    // define materials with textureOnly shader, no light needed
    SLMaterial* matB = new SLMaterial(am,
                                      "matB",
                                      _texB,
                                      nullptr,
                                      nullptr,
                                      nullptr,
                                      SLGLProgramManager::get(SP_textureOnly));
    SLMaterial* matL = new SLMaterial(am,
                                      "matL",
                                      _texL,
                                      nullptr,
                                      nullptr,
                                      nullptr,
                                      SLGLProgramManager::get(SP_textureOnly));
    SLMaterial* matT = new SLMaterial(am,
                                      "matT",
                                      _texT,
                                      nullptr,
                                      nullptr,
                                      nullptr,
                                      SLGLProgramManager::get(SP_textureOnly));
    SLMaterial* matR = new SLMaterial(am,
                                      "matR",
                                      _texR,
                                      nullptr,
                                      nullptr,
                                      nullptr,
                                      SLGLProgramManager::get(SP_textureOnly));

    // build polygons for bottom, left, top & right side
    SLVVec3f VB;
    VB.push_back(SLVec3f(-0.5f, -0.5f, 1.0f));
    VB.push_back(SLVec3f(0.5f, -0.5f, 1.0f));
    VB.push_back(SLVec3f(0.5f, -0.5f, -2.0f));
    VB.push_back(SLVec3f(-0.5f, -0.5f, -2.0f));
    SLVVec2f T;
    T.push_back(SLVec2f(0.0f, 2.0f));
    T.push_back(SLVec2f(0.0f, 0.0f));
    T.push_back(SLVec2f(6.0f, 0.0f));
    T.push_back(SLVec2f(6.0f, 2.0f));
    SLNode* polyB = new SLNode(new SLPolygon(am, VB, T, "PolygonB", matB));

    SLVVec3f VL;
    VL.push_back(SLVec3f(-0.5f, 0.5f, 1.0f));
    VL.push_back(SLVec3f(-0.5f, -0.5f, 1.0f));
    VL.push_back(SLVec3f(-0.5f, -0.5f, -2.0f));
    VL.push_back(SLVec3f(-0.5f, 0.5f, -2.0f));
    SLNode* polyL = new SLNode(new SLPolygon(am, VL, T, "PolygonL", matL));

    SLVVec3f VT;
    VT.push_back(SLVec3f(0.5f, 0.5f, 1.0f));
    VT.push_back(SLVec3f(-0.5f, 0.5f, 1.0f));
    VT.push_back(SLVec3f(-0.5f, 0.5f, -2.0f));
    VT.push_back(SLVec3f(0.5f, 0.5f, -2.0f));
    SLNode* polyT = new SLNode(new SLPolygon(am, VT, T, "PolygonT", matT));

    SLVVec3f VR;
    VR.push_back(SLVec3f(0.5f, -0.5f, 1.0f));
    VR.push_back(SLVec3f(0.5f, 0.5f, 1.0f));
    VR.push_back(SLVec3f(0.5f, 0.5f, -2.0f));
    VR.push_back(SLVec3f(0.5f, -0.5f, -2.0f));
    SLNode* polyR = new SLNode(new SLPolygon(am, VR, T, "PolygonR", matR));

    // 3D Texture Mapping on a pyramid
    SLMaterial* mat3D = new SLMaterial(am,
                                       "mat3D",
                                       _tex3D,
                                       nullptr,
                                       nullptr,
                                       nullptr,
                                       _spr3D);

    // Create 3D textured pyramid mesh and node
    SLMesh* pyramid = new SLMesh(am, "Pyramid");
    pyramid->mat(mat3D);
    pyramid->P          = {{-1, -1, 1},
                           {1, -1, 1},
                           {1, -1, -1},
                           {-1, -1, -1},
                           {0, 2, 0}};
    pyramid->I16        = {0, 3, 1, 1, 3, 2, 4, 0, 1, 4, 1, 2, 4, 2, 3, 4, 3, 0};
    SLNode* pyramidNode = new SLNode(pyramid, "Pyramid");
    pyramidNode->scale(0.2f);
    pyramidNode->translate(0, 0, -3);

    // Create 3D textured sphere mesh and node
    SLNode*   sphere = new SLNode(new SLSphere(am,
                                             0.2f,
                                             16,
                                             16,
                                             "Sphere",
                                             mat3D));
    SLCamera* cam1   = new SLCamera("Camera 1");
    cam1->translation(0, 0, 2.6f);
    cam1->lookAt(0, 0, 0);
    cam1->focalDist(2.2f);
    cam1->background().colors(SLCol4f(0.2f, 0.2f, 0.2f));
    cam1->setInitialState();
    cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);

    SLNode* scene = new SLNode();
    this->root3D(scene);
    scene->addChild(polyB);
    scene->addChild(polyL);
    scene->addChild(polyT);
    scene->addChild(polyR);
    scene->addChild(sphere);
    scene->addChild(cam1);
    sv->camera(cam1);
}
//-----------------------------------------------------------------------------
