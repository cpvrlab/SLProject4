//#############################################################################
//  File:      AppDemoSceneRobot.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneRobot.h>
#include <SLAssetLoader.h>
#include <SLLightDirect.h>
#include <SLRectangle.h>
#include <AppDemo.h>

//-----------------------------------------------------------------------------
AppDemoSceneRobot::AppDemoSceneRobot()
  : SLScene("GLTF File Demo Scene")
{
    info("Fanuc-CRX Robot with forward kinematic movement.");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneRobot::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addNodeToLoad(_robot,
                     AppDemo::modelPath +
                       "GLTF/FanucCRX/Fanuc-CRX.gltf");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneRobot::assemble(SLAssetManager* am, SLSceneView* sv)
{
    // Create a scene group node
    SLNode* scene = new SLNode("scene node");
    this->root3D(scene);

    // Create camera and initialize its parameters
    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(0, 0.5f, 2.0f);
    cam1->lookAt(0, 0.5f, 0);
    cam1->background().colors(SLCol4f(0.7f, 0.7f, 0.7f),
                              SLCol4f(0.2f, 0.2f, 0.2f));
    cam1->focalDist(2);
    cam1->setInitialState();
    scene->addChild(cam1);

    // Define directional
    SLLightDirect* light1 = new SLLightDirect(am,
                                              this,
                                              2,
                                              2,
                                              2,
                                              0.2f,
                                              1,
                                              1,
                                              1);
    light1->lookAt(0, 0, 0);
    light1->attenuation(1, 0, 0);
    light1->createsShadows(true);
    light1->createShadowMap(1,
                            7,
                            SLVec2f(5, 5),
                            SLVec2i(2048, 2048));
    light1->doSmoothShadows(true);
    light1->castsShadows(false);
    scene->addChild(light1);

    SLMaterial* matFloor = new SLMaterial(am,
                                          "matFloor",
                                          SLCol4f::WHITE * 0.5f);
    matFloor->ambient(SLCol4f::WHITE * 0.3f);
    SLMesh* rectangle = new SLRectangle(am,
                                        SLVec2f(-2, -2),
                                        SLVec2f(2, 2),
                                        1,
                                        1,
                                        "rectangle",
                                        matFloor);
    SLNode* floorRect = new SLNode(rectangle);
    floorRect->rotate(90, -1, 0, 0);
    scene->addChild(floorRect);

    // Set missing specular color
    _robot->updateMeshMat([](SLMaterial* m) { m->specular(SLCol4f::WHITE); },
                          true);

    SLNode* crx_j1 = _robot->findChild<SLNode>("crx_j1");
    SLNode* crx_j2 = _robot->findChild<SLNode>("crx_j2");
    SLNode* crx_j3 = _robot->findChild<SLNode>("crx_j3");
    SLNode* crx_j4 = _robot->findChild<SLNode>("crx_j4");
    SLNode* crx_j5 = _robot->findChild<SLNode>("crx_j5");
    SLNode* crx_j6 = _robot->findChild<SLNode>("crx_j6");

    SLfloat angleDEG    = 45;
    SLfloat durationSEC = 3.0f;

    SLAnimation* j1Anim = animManager().createNodeAnimation("j1Anim",
                                                            durationSEC,
                                                            true,
                                                            EC_inOutCubic,
                                                            AL_pingPongLoop);
    j1Anim->createNodeAnimTrackForRotation3(crx_j1,
                                            -angleDEG,
                                            0,
                                            angleDEG,
                                            crx_j1->axisYOS());

    SLAnimation* j2Anim = animManager().createNodeAnimation("j2Anim",
                                                            durationSEC,
                                                            true,
                                                            EC_inOutCubic,
                                                            AL_pingPongLoop);
    j2Anim->createNodeAnimTrackForRotation3(crx_j2,
                                            -angleDEG,
                                            0,
                                            angleDEG,
                                            -crx_j2->axisZOS());

    SLAnimation* j3Anim = animManager().createNodeAnimation("j3Anim",
                                                            durationSEC,
                                                            true,
                                                            EC_inOutCubic,
                                                            AL_pingPongLoop);
    j3Anim->createNodeAnimTrackForRotation3(crx_j3,
                                            angleDEG,
                                            0,
                                            -angleDEG,
                                            -crx_j3->axisZOS());

    SLAnimation* j4Anim = animManager().createNodeAnimation("j4Anim",
                                                            durationSEC,
                                                            true,
                                                            EC_inOutCubic,
                                                            AL_pingPongLoop);
    j4Anim->createNodeAnimTrackForRotation3(crx_j4,
                                            -2 * angleDEG,
                                            0,
                                            2 * angleDEG,
                                            crx_j4->axisXOS());

    SLAnimation* j5Anim = animManager().createNodeAnimation("j5Anim",
                                                            durationSEC,
                                                            true,
                                                            EC_inOutCubic,
                                                            AL_pingPongLoop);
    j5Anim->createNodeAnimTrackForRotation3(crx_j5,
                                            -2 * angleDEG,
                                            0,
                                            2 * angleDEG,
                                            -crx_j5->axisZOS());

    SLAnimation* j6Anim = animManager().createNodeAnimation("j6Anim",
                                                            durationSEC,
                                                            true,
                                                            EC_inOutCubic,
                                                            AL_pingPongLoop);
    j6Anim->createNodeAnimTrackForRotation3(crx_j6,
                                            -2 * angleDEG,
                                            0,
                                            2 * angleDEG,
                                            crx_j6->axisXOS());

    scene->addChild(_robot);

    sv->camera(cam1);
    sv->doWaitOnIdle(true); // Saves energy
}
//-----------------------------------------------------------------------------
