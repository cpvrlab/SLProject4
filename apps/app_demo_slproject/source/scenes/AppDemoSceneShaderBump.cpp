//#############################################################################
//  File:      AppDemoSceneShaderBump.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneShaderBump.h>

#include <SLLightSpot.h>
#include <SLLightDirect.h>
#include <SLRectangle.h>
#include <AppDemo.h>

//-----------------------------------------------------------------------------
AppDemoSceneShaderBump::AppDemoSceneShaderBump()
  : AppScene("Normal Map Bump Mapping")
{
    info("Normal map bump mapping combined with a spot and a directional lighting.");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneShaderBump::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addTextureToLoad(_texC,
                        AppDemo::texturePath +
                          "brickwall0512_C.jpg");
    al.addTextureToLoad(_texN,
                        AppDemo::texturePath +
                          "brickwall0512_N.jpg");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneShaderBump::assemble(SLAssetManager* am, SLSceneView* sv)
{
    // Create materials
    SLMaterial* m1 = new SLMaterial(am, "m1", _texC, _texN);

    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(-10, 10, 10);
    cam1->lookAt(0, 0, 0);
    cam1->focalDist(20);
    cam1->background().colors(SLCol4f(0.5f, 0.5f, 0.5f));
    cam1->setInitialState();

    SLLightSpot* light1 = new SLLightSpot(am, this, 0.3f, 40, true);
    light1->powers(0.1f, 1.0f, 1.0f);
    light1->attenuation(1, 0, 0);
    light1->translation(0, 0, 5);
    light1->lookAt(0, 0, 0);

    SLLightDirect* light2 = new SLLightDirect(am, this);
    light2->ambientColor(SLCol4f(0, 0, 0));
    light2->diffuseColor(SLCol4f(1, 1, 0));
    light2->specularColor(SLCol4f(1, 1, 0));
    light2->translation(-5, -5, 5);
    light2->lookAt(0, 0, 0);
    light2->attenuation(1, 0, 0);

    SLAnimation* anim = this->animManager().createNodeAnimation("light1_anim", 2.0f);
    anim->createNodeAnimTrackForEllipse(light1, 2.0f, A_x, 2.0f, A_Y);

    SLNode* scene = new SLNode;
    this->root3D(scene);
    scene->addChild(light1);
    scene->addChild(light2);
    scene->addChild(new SLNode(new SLRectangle(am, SLVec2f(-5, -5), SLVec2f(5, 5), 1, 1, "Rect", m1)));
    scene->addChild(cam1);

    sv->camera(cam1);
}
//-----------------------------------------------------------------------------
