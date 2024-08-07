/**
 * \file      AppDemoSceneMeshLoad.cpp
 * \brief     Implementation for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see:
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marcus Hudritsch, Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 */

#include <AppDemoSceneMeshLoad.h>
#include <AppCommon.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>
#include <SLRectangle.h>
#include <SLSkybox.h>

//-----------------------------------------------------------------------------
AppDemoSceneMeshLoad::AppDemoSceneMeshLoad() : SLScene("Mesh 3D Loader Test")
{
    info("We use the assimp library to load 3D file formats including materials, skeletons and animations. "
         "You can view the skeleton with key K. You can stop all animations with SPACE key.\n"
         "Switch between perspective and orthographic projection with key 5. "
         "Switch to front view with key 1, to side view with key 3 and to top view with key 7.\n"
         "Try the different stereo rendering modes in the menu Camera.");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneMeshLoad::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addSkyboxToLoad(_skybox,
                       al.modelPath() +
                         "GLTF/glTF-Sample-Models/hdris/approaching_storm_4k.hdr",
                       SLVec2i(512, 512),
                       "HDR Skybox");

    al.addNodeToLoad(_meshGLTF,
                     AppCommon::modelPath +
                       "GLTF/glTF-Sample-Models/2.0/DamagedHelmet/glTF/DamagedHelmet.gltf");
    al.addNodeToLoad(_meshFBX,
                     AppCommon::modelPath +
                       "FBX/Duck/duck.fbx");
    al.addNodeToLoad(_meshDAE,
                     AppCommon::modelPath +
                       "DAE/AstroBoy/AstroBoy.dae");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneMeshLoad::assemble(SLAssetManager* am, SLSceneView* sv)
{
    SLMaterial* matBlu = new SLMaterial(am, "Blue", SLCol4f(0, 0, 0.2f), SLCol4f(1, 1, 1), 100, 0.8f, 0);
    SLMaterial* matRed = new SLMaterial(am, "Red", SLCol4f(0.2f, 0, 0), SLCol4f(1, 1, 1), 100, 0.8f, 0);
    SLMaterial* matGre = new SLMaterial(am, "Green", SLCol4f(0, 0.2f, 0), SLCol4f(1, 1, 1), 100, 0.8f, 0);
    SLMaterial* matGra = new SLMaterial(am, "Gray", SLCol4f(0.3f, 0.3f, 0.3f), SLCol4f(1, 1, 1), 100, 0, 0);

    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->clipNear(.1f);
    cam1->clipFar(30);
    cam1->translation(0, 0, 12);
    cam1->lookAt(0, 0, 0);
    cam1->focalDist(12);
    cam1->stereoEyeSeparation(cam1->focalDist() / 30.0f);
    cam1->background().colors(SLCol4f(0.6f, 0.6f, 0.6f), SLCol4f(0.3f, 0.3f, 0.3f));
    cam1->setInitialState();
    cam1->devRotLoc(&AppCommon::devRot, &AppCommon::devLoc);

    SLLightSpot* light1 = new SLLightSpot(am, this, 2.5f, 2.5f, 2.5f, 0.2f);
    light1->powers(0.1f, 1.0f, 1.0f);
    light1->attenuation(1, 0, 0);
    SLAnimation* anim = this->animManager().createNodeAnimation("anim_light1_backforth", 2.0f, true, EC_inOutQuad, AL_pingPongLoop);
    anim->createNodeAnimTrackForTranslation(light1, SLVec3f(0.0f, 0.0f, -5.0f));

    SLLightSpot* light2 = new SLLightSpot(am, this, -2.5f, -2.5f, 2.5f, 0.2f);
    light2->powers(0.1f, 1.0f, 1.0f);
    light2->attenuation(1, 0, 0);
    anim = this->animManager().createNodeAnimation("anim_light2_updown", 2.0f, true, EC_inOutQuint, AL_pingPongLoop);
    anim->createNodeAnimTrackForTranslation(light2, SLVec3f(0.0f, 5.0f, 0.0f));

    // Start animation
    SLAnimPlayback* charAnim = this->animManager().animPlaybacksBack();
    charAnim->playForward();
    charAnim->playbackRate(0.8f);

    // Scale to so that the AstroBoy is about 2 (meters) high.
    if (_meshGLTF)
    {
        _meshGLTF->scale(0.8f);
        _meshGLTF->translate(-2.4f, 0, -1.5f, TS_object);

        // Update all materials and set their skybox to _skybox
        _meshGLTF->updateMeshMat([=](SLMaterial* m)
                                 { m->skybox(_skybox); },
                                 true);
        _skybox->exposure(4.0f);
    }
    if (_meshDAE)
    {
        _meshDAE->translate(0, -3, 0, TS_object);
        _meshDAE->scale(2.7f);
    }
    if (_meshFBX)
    {
        _meshFBX->scale(0.1f);
        _meshFBX->scale(0.1f);
        _meshFBX->translate(200, 30, -30, TS_object);
        _meshFBX->rotate(-90, 0, 1, 0);
    }

    // define rectangles for the surrounding box
    SLfloat b = 3; // edge size of rectangles
    SLNode *rb, *rl, *rr, *rf, *rt;
    SLuint  res = 20;
    rb          = new SLNode(new SLRectangle(am, SLVec2f(-b, -b), SLVec2f(b, b), res, res, "rectB", matBlu), "rectBNode");
    rb->translate(0, 0, -b, TS_object);
    rl = new SLNode(new SLRectangle(am, SLVec2f(-b, -b), SLVec2f(b, b), res, res, "rectL", matRed), "rectLNode");
    rl->rotate(90, 0, 1, 0);
    rl->translate(0, 0, -b, TS_object);
    rr = new SLNode(new SLRectangle(am, SLVec2f(-b, -b), SLVec2f(b, b), res, res, "rectR", matGre), "rectRNode");
    rr->rotate(-90, 0, 1, 0);
    rr->translate(0, 0, -b, TS_object);
    rf = new SLNode(new SLRectangle(am, SLVec2f(-b, -b), SLVec2f(b, b), res, res, "rectF", matGra), "rectFNode");
    rf->rotate(-90, 1, 0, 0);
    rf->translate(0, 0, -b, TS_object);
    rt = new SLNode(new SLRectangle(am, SLVec2f(-b, -b), SLVec2f(b, b), res, res, "rectT", matGra), "rectTNode");
    rt->rotate(90, 1, 0, 0);
    rt->translate(0, 0, -b, TS_object);

    SLNode* scene = new SLNode("Scene");
    this->root3D(scene);
    this->skybox(_skybox);
    scene->addChild(light1);
    scene->addChild(light2);
    scene->addChild(rb);
    scene->addChild(rl);
    scene->addChild(rr);
    scene->addChild(rf);
    scene->addChild(rt);
    if (_meshGLTF) scene->addChild(_meshGLTF);
    if (_meshFBX) scene->addChild(_meshFBX);
    if (_meshDAE) scene->addChild(_meshDAE);
    scene->addChild(cam1);

    sv->camera(cam1);
}
//-----------------------------------------------------------------------------
