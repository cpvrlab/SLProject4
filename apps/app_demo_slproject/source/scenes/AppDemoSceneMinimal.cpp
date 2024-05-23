//#############################################################################
//  File:      AppDemoSceneMinimal.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneMinimal.h>
#include <SLLightSpot.h>
#include <SLRectangle.h>
#include <AppDemo.h>

//-----------------------------------------------------------------------------
AppDemoSceneMinimal::AppDemoSceneMinimal() : AppScene("Minimal Scene")
{
    info("Minimal scene with a texture mapped rectangle with a point light source.\n"
         "You can find all other test scenes in the menu File > Load Test Scenes."
         "You can jump to the next scene with the Shift-Alt-CursorRight.\n"
         "You can open various info windows under the menu Infos. You can drag, dock and stack them on all sides.\n"
         "You can rotate the scene with click and drag on the left mouse button (LMB).\n"
         "You can zoom in/out with the mousewheel. You can pan with click and drag on the middle mouse button (MMB).\n");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneMinimal::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addTextureToLoad(_texC,
                        AppDemo::texturePath,
                        "earth2048_C.png");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneMinimal::assemble(SLAssetManager* am, SLSceneView* sv)
{
    // Create a scene group node
    SLNode* scene = new SLNode("scene node");
    root3D(scene);

    // Create materials
    SLMaterial* m1 = new SLMaterial(am, "m1", _texC);

    // Create a light source node
    SLLightSpot* light1 = new SLLightSpot(am, this, 0.3f);
    light1->translation(0, 0, 5);
    light1->name("light node");
    scene->addChild(light1);

    // Create meshes and nodes
    SLMesh* rectMesh = new SLRectangle(am,
                                       SLVec2f(-5, -5),
                                       SLVec2f(5, 5),
                                       25,
                                       25,
                                       "rectangle mesh",
                                       m1);
    SLNode* rectNode = new SLNode(rectMesh, "rectangle node");
    scene->addChild(rectNode);

    // Set background color and the root scene node
    sv->sceneViewCamera()->background().colors(SLCol4f(0.7f, 0.7f, 0.7f),
                                               SLCol4f(0.2f, 0.2f, 0.2f));
    // Save energy
    sv->doWaitOnIdle(true);
}
//-----------------------------------------------------------------------------
