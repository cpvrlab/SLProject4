//#############################################################################
//  File:      AppDemoSceneAnimNode.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneAnimNode.h>
#include <AppDemoSceneFigure.h>
#include <AppDemo.h>
#include <SLLightSpot.h>
#include <SLRectangle.h>
#include <SLSphere.h>
#include <SLCylinder.h>

//-----------------------------------------------------------------------------
AppDemoSceneAnimNode::AppDemoSceneAnimNode()
  : AppScene("Node Animation Test Scene")
{
    info("Node animations with different easing curves.");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneAnimNode::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addTextureToLoad(_tex1,
                        AppDemo::texturePath,
                        "Checkerboard0512_C.png");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneAnimNode::assemble(SLAssetManager* am, SLSceneView* sv)
{
    SLMaterial* m1 = new SLMaterial(am, "m1", _tex1);
    m1->kr(0.5f);
    SLMaterial* m2 = new SLMaterial(am, "m2", SLCol4f::WHITE * 0.5, SLCol4f::WHITE, 128, 0.5f, 0.0f, 1.0f);

    SLMesh* floorMesh = new SLRectangle(am, SLVec2f(-5, -5), SLVec2f(5, 5), 20, 20, "FloorMesh", m1);
    SLNode* floorRect = new SLNode(floorMesh);
    floorRect->rotate(90, -1, 0, 0);
    floorRect->translate(0, 0, -5.5f);

    // Bouncing balls
    SLNode* ball1 = new SLNode(new SLSphere(am, 0.3f, 16, 16, "Ball1", m2));
    ball1->translate(0, 0, 4, TS_object);
    SLAnimation* ball1Anim = animManager().createNodeAnimation("Ball1_anim", 1.0f, true, EC_linear, AL_pingPongLoop);
    ball1Anim->createNodeAnimTrackForTranslation(ball1, SLVec3f(0.0f, -5.2f, 0.0f));

    SLNode* ball2 = new SLNode(new SLSphere(am, 0.3f, 16, 16, "Ball2", m2));
    ball2->translate(-1.5f, 0, 4, TS_object);
    SLAnimation* ball2Anim = animManager().createNodeAnimation("Ball2_anim", 1.0f, true, EC_inQuad, AL_pingPongLoop);
    ball2Anim->createNodeAnimTrackForTranslation(ball2, SLVec3f(0.0f, -5.2f, 0.0f));

    SLNode* ball3 = new SLNode(new SLSphere(am, 0.3f, 16, 16, "Ball3", m2));
    ball3->translate(-2.5f, 0, 4, TS_object);
    SLAnimation* ball3Anim = animManager().createNodeAnimation("Ball3_anim", 1.0f, true, EC_outQuad, AL_pingPongLoop);
    ball3Anim->createNodeAnimTrackForTranslation(ball3, SLVec3f(0.0f, -5.2f, 0.0f));

    SLNode* ball4 = new SLNode(new SLSphere(am, 0.3f, 16, 16, "Ball4", m2));
    ball4->translate(1.5f, 0, 4, TS_object);
    SLAnimation* ball4Anim = animManager().createNodeAnimation("Ball4_anim", 1.0f, true, EC_inOutQuad, AL_pingPongLoop);
    ball4Anim->createNodeAnimTrackForTranslation(ball4, SLVec3f(0.0f, -5.2f, 0.0f));

    SLNode* ball5 = new SLNode(new SLSphere(am, 0.3f, 16, 16, "Ball5", m2));
    ball5->translate(2.5f, 0, 4, TS_object);
    SLAnimation* ball5Anim = animManager().createNodeAnimation("Ball5_anim", 1.0f, true, EC_outInQuad, AL_pingPongLoop);
    ball5Anim->createNodeAnimTrackForTranslation(ball5, SLVec3f(0.0f, -5.2f, 0.0f));

    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(0, 0, 22);
    cam1->lookAt(0, 0, 0);
    cam1->focalDist(22);
    cam1->setInitialState();
    cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);

    SLCamera* cam2 = new SLCamera("Camera 2");
    cam2->translation(5, 0, 0);
    cam2->lookAt(0, 0, 0);
    cam2->focalDist(5);
    cam2->clipFar(10);
    cam2->background().colors(SLCol4f(0, 0, 0.6f), SLCol4f(0, 0, 0.3f));
    cam2->setInitialState();
    cam2->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);

    SLCamera* cam3 = new SLCamera("Camera 3");
    cam3->translation(-5, -2, 0);
    cam3->lookAt(0, 0, 0);
    cam3->focalDist(5);
    cam3->clipFar(10);
    cam3->background().colors(SLCol4f(0.6f, 0, 0), SLCol4f(0.3f, 0, 0));
    cam3->setInitialState();
    cam3->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);

    SLLightSpot* light1 = new SLLightSpot(am, this, 0, 2, 0, 0.5f);
    light1->powers(0.2f, 1.0f, 1.0f);
    light1->attenuation(1, 0, 0);
    SLAnimation* light1Anim = this->animManager().createNodeAnimation("Light1_anim", 4.0f);
    light1Anim->createNodeAnimTrackForEllipse(light1, 6, A_z, 6, A_x);

    SLLightSpot* light2 = new SLLightSpot(am, this, 0, 0, 0, 0.2f);
    light2->powers(0.1f, 1.0f, 1.0f);
    light2->attenuation(1, 0, 0);
    light2->translate(-8, -4, 0, TS_world);
    light2->setInitialState();
    SLAnimation*     light2Anim = animManager().createNodeAnimation("light2_anim", 2.0f, true, EC_linear, AL_pingPongLoop);
    SLNodeAnimTrack* track      = light2Anim->createNodeAnimTrack();
    track->animatedNode(light2);
    track->createNodeKeyframe(0.0f);
    track->createNodeKeyframe(1.0f)->translation(SLVec3f(8, 8, 0));
    track->createNodeKeyframe(2.0f)->translation(SLVec3f(16, 0, 0));
    track->translationInterpolation(AI_bezier);

    SLNode* figure = AppDemoSceneFigure::BuildFigureGroup(am, this, m2, true);

    SLNode* scene = new SLNode("Scene");
    root3D(scene);
    scene->addChild(light1);
    scene->addChild(light2);
    scene->addChild(cam1);
    scene->addChild(cam2);
    scene->addChild(cam3);
    scene->addChild(floorRect);
    scene->addChild(ball1);
    scene->addChild(ball2);
    scene->addChild(ball3);
    scene->addChild(ball4);
    scene->addChild(ball5);
    scene->addChild(figure);

    sv->camera(cam1);
}
//-----------------------------------------------------------------------------
