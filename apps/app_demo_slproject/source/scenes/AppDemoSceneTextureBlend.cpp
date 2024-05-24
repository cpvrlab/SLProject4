//#############################################################################
//  File:      AppDemoSceneTextureBlend.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marcus Hudritsch
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneTextureBlend.h>
#include <SLLightSpot.h>
#include <SLPolygon.h>
#include <AppDemo.h>

//-----------------------------------------------------------------------------
AppDemoSceneTextureBlend::AppDemoSceneTextureBlend() : AppScene("Texture Blending Test")
{
    info("Texture map blending with depth sorting. Transparent tree rectangles in view "
         "frustum are rendered back to front. You can turn on/off alpha sorting in the "
         "menu Preferences of press key J.");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneTextureBlend::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addTextureToLoad(_t1,
                        AppDemo::texturePath +
                          "tree1_1024_C.png",
                        GL_LINEAR_MIPMAP_LINEAR,
                        GL_LINEAR,
                        TT_diffuse,
                        GL_CLAMP_TO_EDGE,
                        GL_CLAMP_TO_EDGE);
    al.addTextureToLoad(_t2,
                        AppDemo::texturePath +
                          "grass0512_C.jpg",
                        GL_LINEAR_MIPMAP_LINEAR,
                        GL_LINEAR);
    al.addProgramToLoad(_sp,
                        AppDemo::shaderPath + "PerVrtTm.vert",
                        AppDemo::shaderPath + "PerVrtTm.frag");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneTextureBlend::assemble(SLAssetManager* am, SLSceneView* sv)
{
    SLMaterial* m1 = new SLMaterial(am,
                                    "m1",
                                    SLCol4f(1, 1, 1),
                                    SLCol4f(0, 0, 0),
                                    100);
    SLMaterial* m2 = new SLMaterial(am,
                                    "m2",
                                    SLCol4f(1, 1, 1),
                                    SLCol4f(0, 0, 0),
                                    100);
    m1->program(_sp);
    m1->addTexture(_t1);
    m2->addTexture(_t2);

    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(6.5f, 0.5f, -18);
    cam1->lookAt(0, 0, 0);
    cam1->focalDist(18);
    cam1->background().colors(SLCol4f(0.6f, 0.6f, 1));
    cam1->setInitialState();
    cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);

    SLLightSpot* light = new SLLightSpot(am, this, 0.1f);
    light->translation(5, 5, 5);
    light->lookAt(0, 0, 0);
    light->attenuation(1, 0, 0);

    // Build arrays for polygon vertices and texture coordinates for tree
    SLVVec3f pNW, pSE;
    SLVVec2f tNW, tSE;
    pNW.push_back(SLVec3f(0, 0, 0));
    tNW.push_back(SLVec2f(0.5f, 0.0f));
    pNW.push_back(SLVec3f(1, 0, 0));
    tNW.push_back(SLVec2f(1.0f, 0.0f));
    pNW.push_back(SLVec3f(1, 2, 0));
    tNW.push_back(SLVec2f(1.0f, 1.0f));
    pNW.push_back(SLVec3f(0, 2, 0));
    tNW.push_back(SLVec2f(0.5f, 1.0f));
    pSE.push_back(SLVec3f(-1, 0, 0));
    tSE.push_back(SLVec2f(0.0f, 0.0f));
    pSE.push_back(SLVec3f(0, 0, 0));
    tSE.push_back(SLVec2f(0.5f, 0.0f));
    pSE.push_back(SLVec3f(0, 2, 0));
    tSE.push_back(SLVec2f(0.5f, 1.0f));
    pSE.push_back(SLVec3f(-1, 2, 0));
    tSE.push_back(SLVec2f(0.0f, 1.0f));

    // Build tree out of 4 polygons
    SLNode* p1 = new SLNode(new SLPolygon(am, pNW, tNW, "Tree+X", m1));
    SLNode* p2 = new SLNode(new SLPolygon(am, pNW, tNW, "Tree-Z", m1));
    p2->rotate(90, 0, 1, 0);
    SLNode* p3 = new SLNode(new SLPolygon(am, pSE, tSE, "Tree-X", m1));
    SLNode* p4 = new SLNode(new SLPolygon(am, pSE, tSE, "Tree+Z", m1));
    p4->rotate(90, 0, 1, 0);

    // Turn face culling off so that we see both sides
    p1->drawBits()->on(SL_DB_CULLOFF);
    p2->drawBits()->on(SL_DB_CULLOFF);
    p3->drawBits()->on(SL_DB_CULLOFF);
    p4->drawBits()->on(SL_DB_CULLOFF);

    // Build tree group
    SLNode* tree = new SLNode("grTree");
    tree->addChild(p1);
    tree->addChild(p2);
    tree->addChild(p3);
    tree->addChild(p4);

    // Build arrays for polygon vertices and texcoords for ground
    SLVVec3f pG;
    SLVVec2f tG;
    SLfloat  size = 22.0f;
    pG.push_back(SLVec3f(-size, 0, size));
    tG.push_back(SLVec2f(0, 0));
    pG.push_back(SLVec3f(size, 0, size));
    tG.push_back(SLVec2f(30, 0));
    pG.push_back(SLVec3f(size, 0, -size));
    tG.push_back(SLVec2f(30, 30));
    pG.push_back(SLVec3f(-size, 0, -size));
    tG.push_back(SLVec2f(0, 30));

    SLNode* scene = new SLNode("grScene");
    this->root3D(scene);
    scene->addChild(light);
    scene->addChild(tree);
    scene->addChild(new SLNode(new SLPolygon(am,
                                             pG,
                                             tG,
                                             "Ground",
                                             m2)));

    // create 21*21*21-1 references around the center tree
    SLint res = 10;
    for (SLint iZ = -res; iZ <= res; ++iZ)
    {
        for (SLint iX = -res; iX <= res; ++iX)
        {
            if (iX != 0 || iZ != 0)
            {
                SLNode* t = tree->copyRec();
                t->translate(float(iX) * 2 + Utils::random(0.7f, 1.4f),
                             0,
                             float(iZ) * 2 + Utils::random(0.7f, 1.4f),
                             TS_object);
                t->rotate(Utils::random(0.f, 90.f), 0, 1, 0);
                t->scale(Utils::random(0.5f, 1.0f));
                scene->addChild(t);
            }
        }
    }

    scene->addChild(cam1);

    sv->camera(cam1);
}
//-----------------------------------------------------------------------------
