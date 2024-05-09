//#############################################################################
//  File:      AppDemoSceneShaderWave.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneShaderWave.h>

#include <SLLightSpot.h>
#include <SLSphere.h>
#include <SLRectangle.h>
#include <AppDemo.h>

//-----------------------------------------------------------------------------
AppDemoSceneShaderWave::AppDemoSceneShaderWave()
  : AppScene("Per vertex wave shader")
{

    info("Vertex Shader with wave displacement. "
         "Use H-Key to increment (decrement w. shift) the wave height.");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneShaderWave::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addProgramGenericToLoad(_sp,
                               "Wave.vert",
                               "Wave.frag");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneShaderWave::assemble(SLAssetManager* am, SLSceneView* sv)
{
    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(0, 3, 8);
    cam1->lookAt(0, 0, 0);
    cam1->focalDist(cam1->translationOS().length());
    cam1->background().colors(SLCol4f(0.1f, 0.4f, 0.8f));
    cam1->setInitialState();
    cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);

    // Create generic shader program with 4 custom uniforms
    SLGLUniform1f* u_h = new SLGLUniform1f(UT_const,
                                           "u_h",
                                           0.1f,
                                           0.05f,
                                           0.0f,
                                           0.5f,
                                           (SLKey)'H');
    this->eventHandlers().push_back(u_h);
    _sp->addUniform1f(u_h);
    _sp->addUniform1f(new SLGLUniform1f(UT_inc, "u_t", 0.0f, 0.06f));
    _sp->addUniform1f(new SLGLUniform1f(UT_const, "u_a", 2.5f));
    _sp->addUniform1f(new SLGLUniform1f(UT_incDec, "u_b", 2.2f, 0.01f, 2.0f, 2.5f));

    // Create materials
    SLMaterial* matWater = new SLMaterial(am,
                                          "matWater",
                                          SLCol4f(0.45f, 0.65f, 0.70f),
                                          SLCol4f::WHITE,
                                          300);
    matWater->program(_sp);
    SLMaterial* matRed = new SLMaterial(am,
                                        "matRed",
                                        SLCol4f(1.00f, 0.00f, 0.00f));

    // water rectangle in the y=0 plane
    SLNode* wave = new SLNode(new SLRectangle(am,
                                              SLVec2f(-Utils::PI, -Utils::PI),
                                              SLVec2f(Utils::PI, Utils::PI),
                                              40,
                                              40,
                                              "WaterRect",
                                              matWater));
    wave->rotate(90, -1, 0, 0);

    SLLightSpot* light0 = new SLLightSpot(am, this);
    light0->ambiDiffPowers(0, 1);
    light0->translate(0, 4, -4, TS_object);
    light0->attenuation(1, 0, 0);

    SLNode* scene = new SLNode;
    this->root3D(scene);
    scene->addChild(light0);
    scene->addChild(wave);
    scene->addChild(new SLNode(new SLSphere(am,
                                            1,
                                            32,
                                            32,
                                            "Red Sphere",
                                            matRed)));
    scene->addChild(cam1);

    sv->camera(cam1);
    sv->doWaitOnIdle(false);
}
//-----------------------------------------------------------------------------
