/**
 * \file      AppDemoSceneRTSpheres.cpp
 * \brief     Implementation for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <AppDemoSceneRTSpheres.h>
#include <AppCommon.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>
#include <SLRectangle.h>
#include <SLSphere.h>

//-----------------------------------------------------------------------------
AppDemoSceneRTSpheres::AppDemoSceneRTSpheres(SLSceneID sceneID)
  : SLScene("Ray Tracing Spheres"),
    _sceneID(sceneID)
{
    if (sceneID == SID_RTSpheres)
        info("Classic ray tracing scene with transparent and reflective "
             "spheres. Be patient on mobile devices.");
    else if (sceneID == SID_RTSoftShadows)
    {
        name("Soft Shadow Ray Tracing");
        info("Ray tracing with soft shadow light sampling. Each light "
             "source is sampled 64x per pixel. Be patient on mobile devices.");
    } else
        SL_EXIT_MSG("Should not get here!");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneRTSpheres::registerAssetsToLoad(SLAssetLoader& al)
{
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneRTSpheres::assemble(SLAssetManager* am, SLSceneView* sv)
{

    if (_sceneID == SID_RTSpheres)
    {
        // define materials
        SLMaterial* matGla = new SLMaterial(am,
                                            "Glass",
                                            SLCol4f(0.0f, 0.0f, 0.0f),
                                            SLCol4f(0.5f, 0.5f, 0.5f),
                                            100,
                                            0.4f,
                                            0.6f,
                                            1.5f);
        SLMaterial* matRed = new SLMaterial(am,
                                            "Red",
                                            SLCol4f(0.5f, 0.0f, 0.0f),
                                            SLCol4f(0.5f, 0.5f, 0.5f),
                                            100,
                                            0.5f,
                                            0.0f,
                                            1.0f);
        SLMaterial* matYel = new SLMaterial(am,
                                            "Floor",
                                            SLCol4f(0.8f, 0.6f, 0.2f),
                                            SLCol4f(0.8f, 0.8f, 0.8f),
                                            100,
                                            0.5f,
                                            0.0f,
                                            1.0f);

        SLCamera* cam1 = new SLCamera();
        cam1->translation(0, 0.1f, 2.5f);
        cam1->lookAt(0, 0, 0);
        cam1->focalDist(cam1->translationOS().length());
        cam1->background().colors(SLCol4f(0.1f, 0.4f, 0.8f));
        cam1->setInitialState();
        cam1->devRotLoc(&AppCommon::devRot, &AppCommon::devLoc);

        SLNode* rect = new SLNode(new SLRectangle(am,
                                                  SLVec2f(-3, -3),
                                                  SLVec2f(5, 4),
                                                  20,
                                                  20,
                                                  "Floor",
                                                  matYel));
        rect->rotate(90, -1, 0, 0);
        rect->translate(0, -1, -0.5f, TS_object);

        SLLightSpot* light1 = new SLLightSpot(am,
                                              this,
                                              2,
                                              2,
                                              2,
                                              0.1f);
        light1->powers(1, 7, 7);
        light1->attenuation(0, 0, 1);

        SLLightSpot* light2 = new SLLightSpot(am,
                                              this,
                                              2,
                                              2,
                                              -2,
                                              0.1f);
        light2->powers(1, 7, 7);
        light2->attenuation(0, 0, 1);

        SLNode* scene = new SLNode;
        sv->camera(cam1);
        scene->addChild(light1);
        scene->addChild(light2);
        scene->addChild(SphereGroupRT(am,
                                      3,
                                      0,
                                      0,
                                      0,
                                      1,
                                      30,
                                      matGla,
                                      matRed));
        scene->addChild(rect);
        scene->addChild(cam1);

        root3D(scene);
    }
    else if (_sceneID == SID_RTSoftShadows)
    {
        // Create root node
        SLNode* scene = new SLNode;
        root3D(scene);

        // define materials
        SLCol4f     spec(0.8f, 0.8f, 0.8f);
        SLMaterial* matBlk = new SLMaterial(am,
                                            "Glass",
                                            SLCol4f(0.0f, 0.0f, 0.0f),
                                            SLCol4f(0.5f, 0.5f, 0.5f),
                                            100,
                                            0.5f,
                                            0.5f,
                                            1.5f);
        SLMaterial* matRed = new SLMaterial(am,
                                            "Red",
                                            SLCol4f(0.5f, 0.0f, 0.0f),
                                            SLCol4f(0.5f, 0.5f, 0.5f),
                                            100,
                                            0.5f,
                                            0.0f,
                                            1.0f);
        SLMaterial* matYel = new SLMaterial(am,
                                            "Floor",
                                            SLCol4f(0.8f, 0.6f, 0.2f),
                                            SLCol4f(0.8f, 0.8f, 0.8f),
                                            100,
                                            0.0f,
                                            0.0f,
                                            1.0f);

        SLCamera* cam1 = new SLCamera;
        cam1->translation(0, 0.1f, 4);
        cam1->lookAt(0, 0, 0);
        cam1->focalDist(cam1->translationOS().length());
        cam1->background().colors(SLCol4f(0.1f, 0.4f, 0.8f));
        cam1->setInitialState();
        cam1->devRotLoc(&AppCommon::devRot, &AppCommon::devLoc);
        scene->addChild(cam1);

        SLNode* rect = new SLNode(new SLRectangle(am,
                                                  SLVec2f(-5, -5),
                                                  SLVec2f(5, 5),
                                                  1,
                                                  1,
                                                  "Rect",
                                                  matYel));
        rect->rotate(90, -1, 0, 0);
        rect->translate(0, 0, -0.5f);
        rect->castsShadows(false);
        scene->addChild(rect);

        SLLightSpot* light1 = new SLLightSpot(am,
                                              this,
                                              2,
                                              2,
                                              2,
                                              0.3f);
        light1->samples(8, 8);
        light1->attenuation(0, 0, 1);
        light1->createsShadows(true);
        light1->createShadowMap();
        scene->addChild(light1);

        SLLightSpot* light2 = new SLLightSpot(am,
                                              this,
                                              2,
                                              2,
                                              -2,
                                              0.3f);
        light2->samples(8, 8);
        light2->attenuation(0, 0, 1);
        light2->createsShadows(true);
        light2->createShadowMap();
        scene->addChild(light2);

        scene->addChild(SphereGroupRT(am,
                                      1,
                                      0,
                                      0,
                                      0,
                                      1,
                                      32,
                                      matBlk,
                                      matRed));

        sv->camera(cam1);
    }
}
//-----------------------------------------------------------------------------
//! Creates a recursive sphere group used for the ray tracing scenes
SLNode* AppDemoSceneRTSpheres::SphereGroupRT(SLAssetManager* am,
                                             SLint           depth,
                                             SLfloat         x,
                                             SLfloat         y,
                                             SLfloat         z,
                                             SLfloat         scale,
                                             SLuint          resolution,
                                             SLMaterial*     matGlass,
                                             SLMaterial*     matRed)
{

    SLstring name = matGlass->kt() > 0 ? "GlassSphere" : "RedSphere";
    if (depth == 0)
    {
        SLSphere* sphere  = new SLSphere(am, 0.5f * scale, resolution, resolution, name, matRed);
        SLNode*   sphNode = new SLNode(sphere, "Sphere");
        sphNode->translate(x, y, z, TS_object);
        return sphNode;
    }
    else
    {
        depth--;
        SLNode* sGroup = new SLNode(new SLSphere(am, 0.5f * scale, resolution, resolution, name, matGlass), "SphereGroupRT");
        sGroup->translate(x, y, z, TS_object);
        SLuint newRes = (SLuint)std::max((SLint)resolution - 4, 8);
        sGroup->addChild(SphereGroupRT(am, depth, 0.643951f * scale, 0, 0.172546f * scale, scale / 3, newRes, matRed, matRed));
        sGroup->addChild(SphereGroupRT(am, depth, 0.172546f * scale, 0, 0.643951f * scale, scale / 3, newRes, matRed, matRed));
        sGroup->addChild(SphereGroupRT(am, depth, -0.471405f * scale, 0, 0.471405f * scale, scale / 3, newRes, matRed, matRed));
        sGroup->addChild(SphereGroupRT(am, depth, -0.643951f * scale, 0, -0.172546f * scale, scale / 3, newRes, matRed, matRed));
        sGroup->addChild(SphereGroupRT(am, depth, -0.172546f * scale, 0, -0.643951f * scale, scale / 3, newRes, matRed, matRed));
        sGroup->addChild(SphereGroupRT(am, depth, 0.471405f * scale, 0, -0.471405f * scale, scale / 3, newRes, matRed, matRed));
        sGroup->addChild(SphereGroupRT(am, depth, 0.272166f * scale, 0.544331f * scale, 0.272166f * scale, scale / 3, newRes, matRed, matRed));
        sGroup->addChild(SphereGroupRT(am, depth, -0.371785f * scale, 0.544331f * scale, 0.099619f * scale, scale / 3, newRes, matRed, matRed));
        sGroup->addChild(SphereGroupRT(am, depth, 0.099619f * scale, 0.544331f * scale, -0.371785f * scale, scale / 3, newRes, matRed, matRed));
        return sGroup;
    }
}
//-----------------------------------------------------------------------------
