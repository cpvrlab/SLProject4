//#############################################################################
//  File:      AppDemoSceneAnimSkinnedMass2.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneAnimSkinnedMass2.h>
#include <AppDemo.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>
#include <SLRectangle.h>

//-----------------------------------------------------------------------------
AppDemoSceneAnimSkinnedMass2::AppDemoSceneAnimSkinnedMass2()
  : SLScene("Mass Skeletal Animation Test Scene")
{
    SLchar name[512];
    snprintf(name,
             sizeof(name),
             "Massive Skinned Animation Benchmark w. %d individual Astroboys",
             _size * _size);
    info(name);
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneAnimSkinnedMass2::registerAssetsToLoad(SLAssetLoader& al)
{
    SLuint iA = 0;
    for (SLint iZ = 0; iZ < _size; ++iZ)
    {
        for (SLint iX = 0; iX < _size; ++iX)
        {
            al.addNodeToLoad(_astroboy[iA++],
                             AppDemo::modelPath +
                               "DAE/AstroBoy/AstroBoy.dae");
        }
    }
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneAnimSkinnedMass2::assemble(SLAssetManager* am,
                                            SLSceneView*    sv)
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
    cam1->translation(0, 30, 0);
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

    // Assemble scene
    SLNode* scene = new SLNode("scene group");
    root3D(scene);
    scene->addChild(light1);
    scene->addChild(rect);
    scene->addChild(cam1);

    // create army with individual astroboys
    SLfloat offset = 1.0f;
    SLfloat z      = (float)(_size - 1) * offset * 0.5f;
    SLuint  iA     = 0;

    for (SLint iZ = 0; iZ < _size; ++iZ)
    {
        SLfloat x = -(float)(_size - 1) * offset * 0.5f;

        for (SLint iX = 0; iX < _size; ++iX)
        {
            SLAnimPlayback* anim = animManager().animPlaybackByIndex(iA);
            anim->playForward();
            anim->playbackRate(Utils::random(0.5f, 1.5f));
            _astroboy[iA]->translate(x, 0, z, TS_object);
            scene->addChild(_astroboy[iA]);
            x += offset;
            iA++;
        }
        z -= offset;
    }

    sv->camera(cam1);
}
//-----------------------------------------------------------------------------
