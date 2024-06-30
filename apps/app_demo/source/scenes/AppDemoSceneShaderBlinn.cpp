/**
 * \file      AppDemoSceneShaderBlinn.cpp
 * \brief     Implementation for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <AppDemoSceneShaderBlinn.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>
#include <SLLightRect.h>
#include <SLSphere.h>
#include <AppCommon.h>

//-----------------------------------------------------------------------------
AppDemoSceneShaderBlinn::AppDemoSceneShaderBlinn(SLstring name, bool perVertex)
  : SLScene(name),
    _perVertex(perVertex)
{
    if (_perVertex)
    {
        info("Per-vertex lighting with Blinn-Phong reflection model. "
             "The reflection of 5 light sources is calculated per vertex. "
             "The green and the white light are attached to the camera, the others are in the scene. "
             "The light calculation per vertex is the fastest but leads to artefacts with spot lights");
    }
    else
    {
        info("Per-pixel lighting with Blinn-Phong reflection model. "
             "The reflection of 5 light sources is calculated per pixel. "
             "The light calculation is done in the fragment shader.");
    }
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneShaderBlinn::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addTextureToLoad(_texC,
                        AppCommon::texturePath +
                          "earth2048_C_Q95.jpg");
    al.addTextureToLoad(_texN,
                        AppCommon::texturePath +
                          "earth2048_N.jpg");
    al.addTextureToLoad(_texH,
                        AppCommon::texturePath +
                          "earth2048_H.jpg");
    al.addProgramToLoad(_perVrtTm,
                        AppCommon::shaderPath + "PerVrtBlinnTm.vert",
                        AppCommon::shaderPath + "PerVrtBlinnTm.frag");
    al.addProgramToLoad(_perVrt,
                        AppCommon::shaderPath + "PerVrtBlinn.vert",
                        AppCommon::shaderPath + "PerVrtBlinn.frag");
    al.addProgramToLoad(_perPix,
                        AppCommon::shaderPath + "PerPixBlinnTmNm.vert",
                        AppCommon::shaderPath + "PerPixBlinnTmPm.frag");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneShaderBlinn::assemble(SLAssetManager* am, SLSceneView* sv)
{
    SLMaterial* mL = nullptr;
    SLMaterial* mM = nullptr;
    SLMaterial* mR = nullptr;

    if (_perVertex)
    {
        mL = new SLMaterial(am, "mL", _texC, nullptr, nullptr, nullptr, _perVrtTm);
        mM = new SLMaterial(am, "mM", _perVrt);
        mR = new SLMaterial(am, "mR", _texC, nullptr, nullptr, nullptr, _perVrtTm);
    }
    else
    { // per pixel
        SLGLUniform1f* scale  = new SLGLUniform1f(UT_const, "u_scale", 0.02f, 0.002f, 0, 1);
        SLGLUniform1f* offset = new SLGLUniform1f(UT_const, "u_offset", -0.02f, 0.002f, -1, 1);
        _perPix->addUniform1f(scale);
        _perPix->addUniform1f(offset);
        mL = new SLMaterial(am, "mL", _texC);
        mM = new SLMaterial(am, "mM");
        mR = new SLMaterial(am, "mR", _texC, _texN, _texH, nullptr, _perPix);
    }

    mM->shininess(500);

    // Base root group node for the scene
    SLNode* scene = new SLNode;
    this->root3D(scene);

    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(0, 0, 7);
    cam1->lookAt(0, 0, 0);
    cam1->focalDist(7);
    cam1->background().colors(SLCol4f(0.1f, 0.1f, 0.1f));
    cam1->setInitialState();
    scene->addChild(cam1);

    // Define 5 light sources

    // A rectangular white light attached to the camera
    SLLightRect* lightW = new SLLightRect(am, this, 2.0f, 1.0f);
    lightW->ambiDiffPowers(0, 5);
    lightW->translation(0, 2.5f, 0);
    lightW->translation(0, 2.5f, -7);
    lightW->rotate(-90, 1, 0, 0);
    lightW->attenuation(0, 0, 1);
    cam1->addChild(lightW);

    // A red point light from the front attached in the scene
    SLLightSpot* lightR = new SLLightSpot(am, this, 0.1f);
    lightR->ambientColor(SLCol4f(0, 0, 0));
    lightR->diffuseColor(SLCol4f(1, 0, 0));
    lightR->specularColor(SLCol4f(1, 0, 0));
    lightR->translation(0, 0, 2);
    lightR->lookAt(0, 0, 0);
    lightR->attenuation(0, 0, 1);
    scene->addChild(lightR);

    // A green spot head light with 40 deg. spot angle from front right
    SLLightSpot* lightG = new SLLightSpot(am, this, 0.1f, 20, true);
    lightG->ambientColor(SLCol4f(0, 0, 0));
    lightG->diffuseColor(SLCol4f(0, 1, 0));
    lightG->specularColor(SLCol4f(0, 1, 0));
    lightG->translation(1.5f, 1, -5.5f);
    lightG->lookAt(0, 0, -7);
    lightG->attenuation(1, 0, 0);
    cam1->addChild(lightG);

    // A blue spot light with 40 deg. spot angle from front left
    SLLightSpot* lightB = new SLLightSpot(am, this, 0.1f, 20.0f, true);
    lightB->ambientColor(SLCol4f(0, 0, 0));
    lightB->diffuseColor(SLCol4f(0, 0, 1));
    lightB->specularColor(SLCol4f(0, 0, 1));
    lightB->translation(-1.5f, 1.5f, 1.5f);
    lightB->lookAt(0, 0, 0);
    lightB->attenuation(1, 0, 0);
    SLAnimation* light3Anim = animManager().createNodeAnimation("Ball3_anim",
                                                                1.0f,
                                                                true,
                                                                EC_outQuad,
                                                                AL_pingPongLoop);
    light3Anim->createNodeAnimTrackForTranslation(lightB, SLVec3f(0, -2, 0));
    scene->addChild(lightB);

    // A yellow directional light from the back-bottom
    // Do constant attenuation for directional lights since it is infinitely far away
    SLLightDirect* lightY = new SLLightDirect(am, this);
    lightY->ambientColor(SLCol4f(0, 0, 0));
    lightY->diffuseColor(SLCol4f(1, 1, 0));
    lightY->specularColor(SLCol4f(1, 1, 0));
    lightY->translation(-1.5f, -1.5f, 1.5f);
    lightY->lookAt(0, 0, 0);
    lightY->attenuation(1, 0, 0);
    scene->addChild(lightY);

    // Add some meshes to be lighted
    SLNode* sphereL = new SLNode(new SLSpheric(am, 1.0f, 0.0f, 180.0f, 36, 36, "Sphere", mL));
    sphereL->translate(-2, 0, 0);
    sphereL->rotate(90, -1, 0, 0);
    SLNode* sphereM = new SLNode(new SLSpheric(am, 1.0f, 0.0f, 180.0f, 36, 36, "Sphere", mM));
    SLNode* sphereR = new SLNode(new SLSpheric(am, 1.0f, 0.0f, 180.0f, 36, 36, "Sphere", mR));
    sphereR->translate(2, 0, 0);
    sphereR->rotate(90, -1, 0, 0);

    scene->addChild(sphereL);
    scene->addChild(sphereM);
    scene->addChild(sphereR);
    sv->camera(cam1);
}
//-----------------------------------------------------------------------------
