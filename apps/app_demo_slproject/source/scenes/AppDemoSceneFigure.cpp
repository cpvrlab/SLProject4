//#############################################################################
//  File:      AppDemoSceneFigure.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneFigure.h>
#include <AppDemo.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>
#include <SLBox.h>
#include <SLRectangle.h>
#include <SLSphere.h>
#include <SLCylinder.h>

//-----------------------------------------------------------------------------
AppDemoSceneFigure::AppDemoSceneFigure() : SLScene("Hierarchical Figure Test")
{
    info("Hierarchical scenegraph with multiple subgroups in the figure. "
         "The goal is design a figure with hierarchical transforms containing only rotations and translations. "
         "You can see the hierarchy better in the Scenegraph window. In there the nodes are white and the meshes yellow. "
         "You can view the axis aligned bounding boxes with key B and the nodes origin and axis with key X.");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneFigure::registerAssetsToLoad(SLAssetLoader& al)
{
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneFigure::assemble(SLAssetManager* am, SLSceneView* sv)
{
    // Create textures and materials
    SLMaterial* m1 = new SLMaterial(am, "m1", SLCol4f::BLACK, SLCol4f::WHITE, 128, 0.2f, 0.8f, 1.5f);
    SLMaterial* m2 = new SLMaterial(am, "m2", SLCol4f::WHITE * 0.3f, SLCol4f::WHITE, 128, 0.5f, 0.0f, 1.0f);

    SLuint  res       = 20;
    SLMesh* rectangle = new SLRectangle(am, SLVec2f(-5, -5), SLVec2f(5, 5), res, res, "rectangle", m2);
    SLNode* floorRect = new SLNode(rectangle);
    floorRect->rotate(90, -1, 0, 0);
    floorRect->translate(0, 0, -5.5f);

    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(-7, 2, 7);
    cam1->lookAt(0, -2, 0);
    cam1->focalDist(10);
    cam1->setInitialState();
    cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);
    cam1->background().colors(SLCol4f(0.7f, 0.6f, 1.0f),
                              SLCol4f(0.1f, 0.4f, 0.8f));

    SLLightSpot* light1 = new SLLightSpot(am, this, 5, 0, 5, 0.5f);
    light1->powers(0.2f, 0.9f, 0.9f);
    light1->attenuation(1, 0, 0);

    SLNode* figure = BuildFigureGroup(am, this, m1, true);

    SLNode* scene = new SLNode("scene node");
    this->root3D(scene);
    scene->addChild(light1);
    scene->addChild(cam1);
    scene->addChild(floorRect);
    scene->addChild(figure);

    sv->camera(cam1);
}

//-----------------------------------------------------------------------------
//! Build a hierarchical figurine with arms and legs
SLNode* AppDemoSceneFigure::BuildFigureGroup(SLAssetManager* am,
                                             SLScene*        s,
                                             SLMaterial*     mat,
                                             SLbool          withAnimation)
{
    SLNode* cyl;
    SLuint  res = 16;

    // Feet
    SLNode* feet = new SLNode("feet group (T13,R6)");
    feet->addMesh(new SLSphere(am, 0.2f, 16, 16, "ankle", mat));
    SLNode* feetbox = new SLNode(new SLBox(am,
                                           -0.2f,
                                           -0.1f,
                                           0.0f,
                                           0.2f,
                                           0.1f,
                                           0.8f,
                                           "foot mesh",
                                           mat),
                                 "feet (T14)");
    feetbox->translate(0.0f, -0.25f, -0.15f, TS_object);
    feet->addChild(feetbox);
    feet->translate(0.0f, 0.0f, 1.6f, TS_object);
    feet->rotate(-90.0f, 1.0f, 0.0f, 0.0f);

    // Assemble low leg
    SLNode* leglow = new SLNode("low leg group (T11, R5)");
    leglow->addMesh(new SLSphere(am, 0.3f, res, res, "knee mesh", mat));
    cyl = new SLNode(new SLCylinder(am,
                                    0.2f,
                                    1.4f,
                                    1,
                                    res,
                                    false,
                                    false,
                                    "shin mesh",
                                    mat),
                     "shin (T12)");
    cyl->translate(0.0f, 0.0f, 0.2f, TS_object);
    leglow->addChild(cyl);
    leglow->addChild(feet);
    leglow->translate(0.0f, 0.0f, 1.27f, TS_object);
    leglow->rotate(0, 1.0f, 0.0f, 0.0f);

    // Assemble leg
    SLNode* leg = new SLNode("leg group");
    leg->addMesh(new SLSphere(am, 0.4f, res, res, "hip joint mesh", mat));
    cyl = new SLNode(new SLCylinder(am,
                                    0.3f,
                                    1.0f,
                                    1,
                                    res,
                                    false,
                                    false,
                                    "thigh mesh",
                                    mat),
                     "thigh (T10)");
    cyl->translate(0.0f, 0.0f, 0.27f, TS_object);
    leg->addChild(cyl);
    leg->addChild(leglow);

    // Assemble left & right leg
    SLNode* legLeft = new SLNode("left leg group (T8)");
    legLeft->translate(-0.4f, 0.0f, 2.2f, TS_object);
    legLeft->addChild(leg);
    SLNode* legRight = new SLNode("right leg group (T9)");
    legRight->translate(0.4f, 0.0f, 2.2f, TS_object);
    legRight->addChild(leg->copyRec());

    // Assemble low arm
    SLNode* armlow = new SLNode("low arm group (T6,R4)");
    armlow->addMesh(new SLSphere(am, 0.2f, 16, 16, "elbow mesh", mat));
    cyl = new SLNode(new SLCylinder(am, 0.15f, 1.0f, 1, res, true, false, "low arm mesh", mat), "low arm (T7)");
    cyl->translate(0.0f, 0.0f, 0.14f, TS_object);
    armlow->addChild(cyl);
    armlow->translate(0.0f, 0.0f, 1.2f, TS_object);
    armlow->rotate(45, -1.0f, 0.0f, 0.0f);

    // Assemble arm
    SLNode* arm = new SLNode("arm group");
    arm->addMesh(new SLSphere(am, 0.3f, 16, 16, "shoulder mesh", mat));
    cyl = new SLNode(new SLCylinder(am, 0.2f, 1.0f, 1, res, false, false, "upper arm mesh", mat), "upper arm (T5)");
    cyl->translate(0.0f, 0.0f, 0.2f, TS_object);
    arm->addChild(cyl);
    arm->addChild(armlow);

    // Assemble left & right arm
    SLNode* armLeft = new SLNode("left arm group (T3,R2)");
    armLeft->translate(-1.1f, 0.0f, 0.3f, TS_object);
    armLeft->rotate(10, -1, 0, 0);
    armLeft->addChild(arm);
    SLNode* armRight = new SLNode("right arm group (T4,R3)");
    armRight->translate(1.1f, 0.0f, 0.3f, TS_object);
    armRight->rotate(-60, -1, 0, 0);
    armRight->addChild(arm->copyRec());

    // Assemble head & neck
    SLNode* head = new SLNode(new SLSphere(am, 0.5f, res, res, "head mesh", mat), "head (T1)");
    head->translate(0.0f, 0.0f, -0.7f, TS_object);
    SLSphere* eye  = new SLSphere(am, 0.06f, res, res, "eye mesh", mat);
    SLNode*   eyeL = new SLNode(eye, SLVec3f(-0.15f, 0.48f, 0), "eyeL (T1.1)");
    SLNode*   eyeR = new SLNode(eye, SLVec3f(0.15f, 0.48f, 0), "eyeR (T1.2)");
    head->addChild(eyeL);
    head->addChild(eyeR);
    SLNode* neck = new SLNode(new SLCylinder(am, 0.25f, 0.3f, 1, res, false, false, "neck mesh", mat), "neck (T2)");
    neck->translate(0.0f, 0.0f, -0.3f, TS_object);

    // Assemble figure Left
    SLNode* figure = new SLNode("figure group (R1)");
    figure->addChild(new SLNode(new SLBox(am, -0.8f, -0.4f, 0.0f, 0.8f, 0.4f, 2.0f, "chest mesh", mat), "chest"));
    figure->addChild(head);
    figure->addChild(neck);
    figure->addChild(armLeft);
    figure->addChild(armRight);
    figure->addChild(legLeft);
    figure->addChild(legRight);
    figure->rotate(90, 1, 0, 0);

    // Add animations for left leg
    if (withAnimation)
    {
        legLeft = figure->findChild<SLNode>("left leg group (T8)");
        legLeft->rotate(30, -1, 0, 0);
        SLAnimation* anim = s->animManager().createNodeAnimation("figure animation", 2.0f, true, EC_inOutQuint, AL_pingPongLoop);
        anim->createNodeAnimTrackForRotation(legLeft, 60, SLVec3f(1, 0, 0));

        SLNode* legLowLeft = legLeft->findChild<SLNode>("low leg group (T11, R5)");
        anim->createNodeAnimTrackForRotation(legLowLeft, 40, SLVec3f(1, 0, 0));

        SLNode* feetLeft = legLeft->findChild<SLNode>("feet group (T13,R6)");
        anim->createNodeAnimTrackForRotation(feetLeft, 40, SLVec3f(1, 0, 0));

        armLeft = figure->findChild<SLNode>("left arm group (T3,R2)");
        armLeft->rotate(-45, -1, 0, 0);
        anim->createNodeAnimTrackForRotation(armLeft, -60, SLVec3f(1, 0, 0));

        armRight = figure->findChild<SLNode>("right arm group (T4,R3)");
        armRight->rotate(45, -1, 0, 0);
        anim->createNodeAnimTrackForRotation(armRight, 60, SLVec3f(1, 0, 0));
    }

    return figure;
}
//-----------------------------------------------------------------------------
