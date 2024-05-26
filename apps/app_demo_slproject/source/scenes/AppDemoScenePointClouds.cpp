//#############################################################################
//  File:      AppDemoScenePointClouds.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoScenePointClouds.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>
#include <SLPoints.h>
#include <AppDemo.h>

//-----------------------------------------------------------------------------
AppDemoScenePointClouds::AppDemoScenePointClouds() : SLScene("Point Cloud Test Scene")
{
    info("Point Clouds with normal and uniform distribution. "
         "You can select vertices with rectangle select (CTRL-LMB) "
         "and deselect selected with ALT-LMB.");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoScenePointClouds::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addProgramToLoad(_sp1,
                        AppDemo::shaderPath + "ColorUniformPoint.vert",
                        AppDemo::shaderPath + "Color.frag");
    al.addProgramToLoad(_sp2,
                        AppDemo::shaderPath + "ColorUniformPoint.vert",
                        AppDemo::shaderPath + "Color.frag");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoScenePointClouds::assemble(SLAssetManager* am, SLSceneView* sv)
{
    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->clipNear(0.1f);
    cam1->clipFar(100);
    cam1->translation(0, 0, 15);
    cam1->lookAt(0, 0, 0);
    cam1->focalDist(15);
    cam1->background().colors(SLCol4f(0.1f, 0.1f, 0.1f));
    cam1->setInitialState();
    cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);

    SLLightSpot* light1 = new SLLightSpot(am, this, 10, 10, 10, 0.3f);
    light1->powers(0.2f, 0.8f, 1.0f);
    light1->attenuation(1, 0, 0);

    SLMaterial* pcMat1 = new SLMaterial(am, "Red", SLCol4f::RED);
    pcMat1->program(_sp1);
    pcMat1->program()->addUniform1f(new SLGLUniform1f(UT_const, "u_pointSize", 4.0f));
    SLRnd3fNormal rndN(SLVec3f(0, 0, 0), SLVec3f(5, 2, 1));
    SLNode*       pc1 = new SLNode(new SLPoints(am, 1000, rndN, "PC1", pcMat1));
    pc1->translate(-5, 0, 0);

    SLMaterial* pcMat2 = new SLMaterial(am, "Green", SLCol4f::GREEN);
    pcMat2->program(_sp2);
    pcMat2->program()->addUniform1f(new SLGLUniform1f(UT_const, "u_pointSize", 1.0f));
    SLRnd3fUniform rndU(SLVec3f(0, 0, 0), SLVec3f(2, 3, 5));
    SLNode*        pc2 = new SLNode(new SLPoints(am, 1000, rndU, "PC2", pcMat2));
    pc2->translate(5, 0, 0);

    SLNode* scene = new SLNode("scene");
    this->root3D(scene);
    scene->addChild(cam1);
    scene->addChild(light1);
    scene->addChild(pc1);
    scene->addChild(pc2);

    sv->camera(cam1);
    sv->doWaitOnIdle(false);
}
//-----------------------------------------------------------------------------
