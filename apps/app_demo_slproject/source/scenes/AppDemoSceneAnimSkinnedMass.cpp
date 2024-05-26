//#############################################################################
//  File:      AppDemoSceneAnimSkinnedMass.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneAnimSkinnedMass.h>
#include <AppDemo.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>
#include <SLRectangle.h>

//-----------------------------------------------------------------------------
AppDemoSceneAnimSkinnedMass::AppDemoSceneAnimSkinnedMass()
  : SLScene("Mass Skeletal Animation Test Scene")
{
    info("Mass Skeletal Animation Test Scene");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneAnimSkinnedMass::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addNodeToLoad(_center,
                     AppDemo::modelPath +
                     "DAE/AstroBoy/AstroBoy.dae");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneAnimSkinnedMass::assemble(SLAssetManager* am, SLSceneView* sv)
{
    // Create materials
    SLMaterial* m1 = new SLMaterial(am, "m1", SLCol4f::GRAY);
    m1->specular(SLCol4f::BLACK);

    // Define a light
    SLLightSpot* light1 = new SLLightSpot(am,
                                          this,
                                          100,
                                          40,
                                          100,
                                          1);
    light1->powers(0.1f, 1.0f, 1.0f);
    light1->attenuation(1, 0, 0);

    // Define camera
    SLCamera* cam1 = new SLCamera;
    cam1->translation(0, 10, 10);
    cam1->lookAt(0, 0, 0);
    cam1->focalDist(cam1->translationOS().length());
    cam1->background().colors(SLCol4f(0.1f, 0.4f, 0.8f));
    cam1->setInitialState();
    cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);

    // Floor rectangle
    SLNode* rect = new SLNode(new SLRectangle(am,
                                              SLVec2f(-20, -20),
                                              SLVec2f(20, 20),
                                              SLVec2f(0, 0),
                                              SLVec2f(50, 50),
                                              50,
                                              50,
                                              "Floor",
                                              m1));
    rect->rotate(90, -1, 0, 0);

    animManager().lastAnimPlayback()->playForward();

    // Assemble scene
    SLNode* scene = new SLNode("scene group");
    root3D(scene);
    scene->addChild(light1);
    scene->addChild(rect);
    scene->addChild(_center);
    scene->addChild(cam1);

    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    std::default_random_engine            randEngine;

    // create astroboys around the center astroboy
    SLint size = 4;
    for (SLint iZ = -size; iZ <= size; ++iZ)
    {
        for (SLint iX = -size; iX <= size; ++iX)
        {
            SLbool shift = iX % 2 != 0;
            if (iX != 0 || iZ != 0)
            {
                float   xt = float(iX) * 1.0f;
                float   zt = float(iZ) * 1.0f + ((shift) ? 0.5f : 0.0f);
                SLNode* n  = _center->copyRec();
                n->translate(xt, 0, zt, TS_object);
                n->scale(0.75f + 0.5f * dist(randEngine));
                scene->addChild(n);
            }
        }
    }

    sv->camera(cam1);
}
//-----------------------------------------------------------------------------
