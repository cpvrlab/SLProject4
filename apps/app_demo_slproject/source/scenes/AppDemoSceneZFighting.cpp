//#############################################################################
//  File:      AppDemoSceneZFighting.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneZFighting.h>
#include <AppDemo.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>
#include <SLRectangle.h>

//-----------------------------------------------------------------------------
AppDemoSceneZFighting::AppDemoSceneZFighting() : SLScene("Z-Fighting Test Scene")
{
    info("The reason for this depth fighting is that the camera's near clipping distance "
         "is almost zero and the far clipping distance is too large. The depth buffer only "
         "has 32-bit precision, which leads to this fighting effect when the distance "
         "between the near and far clipping planes is too large. You can adjust these "
         "values over the menu Camera > Projection");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneZFighting::registerAssetsToLoad(SLAssetLoader& al)
{
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneZFighting::assemble(SLAssetManager* am, SLSceneView* sv)
{
    // Create a scene group node
    SLNode* scene = new SLNode("scene node");
    this->root3D(scene);

    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->clipNear(0.0001f);
    cam1->clipFar(1000000);
    cam1->translation(0, 0, 4);
    cam1->lookAt(0, 0, 0);
    cam1->focalDist(4);
    cam1->background().colors(SLCol4f(0.7f, 0.7f, 0.7f),
                              SLCol4f(0.2f, 0.2f, 0.2f));
    cam1->setInitialState();
    scene->addChild(cam1);

    // Create materials
    SLMaterial* matR = new SLMaterial(am, "matR", SLCol4f::RED);
    SLMaterial* matG = new SLMaterial(am, "matG", SLCol4f::GREEN);

    // Create a light source node
    SLLightSpot* light1 = new SLLightSpot(am, this, 0.3f);
    light1->translation(5, 0, 5);
    light1->name("light node");
    scene->addChild(light1);

    // Create two squares
    SLMesh* rectMeshR = new SLRectangle(am,
                                        SLVec2f(-1, -1),
                                        SLVec2f(1, 1),
                                        1,
                                        1,
                                        "RectR",
                                        matR);
    SLNode* rectNodeR = new SLNode(rectMeshR, "Rect Node Red");
    scene->addChild(rectNodeR);

    SLMesh* rectMeshG = new SLRectangle(am,
                                        SLVec2f(-0.8f, -0.8f),
                                        SLVec2f(0.8f, 0.8f),
                                        1,
                                        1,
                                        "RectG",
                                        matG);
    SLNode* rectNodeG = new SLNode(rectMeshG, "Rect Node Green");
    rectNodeG->rotate(2.0f, 1, 1, 0);
    scene->addChild(rectNodeG);

    // Save energy
    sv->doWaitOnIdle(true);

    sv->camera(cam1);
}
//-----------------------------------------------------------------------------
