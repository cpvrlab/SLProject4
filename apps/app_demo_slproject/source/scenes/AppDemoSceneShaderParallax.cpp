//#############################################################################
//  File:      AppDemoSceneShaderParallax.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneShaderParallax.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>
#include <SLLightDirect.h>
#include <SLRectangle.h>
#include <AppDemo.h>

//-----------------------------------------------------------------------------
AppDemoSceneShaderParallax::AppDemoSceneShaderParallax()
  : SLScene("Parallax Bump Mapping Test")
{
    info("Normal map parallax mapping with a spot and a directional light"
         "Use X-Key to increment (decrement w. shift) parallax scale."
         "Use O-Key to increment (decrement w. shift) parallax offset.\n");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneShaderParallax::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addTextureToLoad(_texC,
                        AppDemo::texturePath +
                          "brickwall0512_C.jpg");
    al.addTextureToLoad(_texN,
                        AppDemo::texturePath +
                          "brickwall0512_N.jpg");
    al.addTextureToLoad(_texH,
                        AppDemo::texturePath +
                          "brickwall0512_H.jpg");
    al.addProgramToLoad(_sp,
                        AppDemo::shaderPath + "PerPixBlinnTmNm.vert",
                        AppDemo::shaderPath + "PerPixBlinnTmPm.frag");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneShaderParallax::assemble(SLAssetManager* am, SLSceneView* sv)
{
    SLGLUniform1f* scale  = new SLGLUniform1f(UT_const,
                                             "u_scale",
                                             0.04f,
                                             0.002f,
                                             0,
                                             1,
                                             (SLKey)'X');
    SLGLUniform1f* offset = new SLGLUniform1f(UT_const,
                                              "u_offset",
                                              -0.03f,
                                              0.002f,
                                              -1,
                                              1,
                                              (SLKey)'O');
    this->eventHandlers().push_back(scale);
    this->eventHandlers().push_back(offset);
    _sp->addUniform1f(scale);
    _sp->addUniform1f(offset);

    // Create materials
    SLMaterial* m1 = new SLMaterial(am,
                                    "mat1",
                                    _texC,
                                    _texN,
                                    _texH,
                                    nullptr,
                                    _sp);

    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(-10, 10, 10);
    cam1->lookAt(0, 0, 0);
    cam1->focalDist(20);
    cam1->background().colors(SLCol4f(0.5f, 0.5f, 0.5f));
    cam1->setInitialState();

    SLLightSpot* light1 = new SLLightSpot(am,
                                          this,
                                          0.3f,
                                          40,
                                          true);
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
    anim->createNodeAnimTrackForEllipse(light1,
                                        2.0f,
                                        A_x,
                                        2.0f,
                                        A_Y);

    SLNode* scene = new SLNode;
    this->root3D(scene);
    scene->addChild(light1);
    scene->addChild(light2);
    scene->addChild(new SLNode(new SLRectangle(am,
                                               SLVec2f(-5, -5),
                                               SLVec2f(5, 5),
                                               1,
                                               1,
                                               "Rect",
                                               m1)));
    scene->addChild(cam1);

    sv->camera(cam1);
}
//-----------------------------------------------------------------------------
