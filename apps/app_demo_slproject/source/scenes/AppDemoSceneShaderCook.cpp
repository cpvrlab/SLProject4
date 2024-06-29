/**
 * \file      AppDemoSceneShaderCook.cpp
 * \brief     Implementation for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <AppDemoSceneShaderCook.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>
#include <SLLightDirect.h>
#include <SLSphere.h>
#include <AppDemo.h>

//-----------------------------------------------------------------------------
AppDemoSceneShaderCook::AppDemoSceneShaderCook()
  : SLScene("Cook-Torrance Shading")
{
    info("Cook-Torrance reflection model. Left-Right: roughness 0.05-1, Top-Down: metallic: 1-0. "
         "The center sphere has roughness and metallic encoded in textures. "
         "The reflection model produces a more physically based light reflection "
         "than the standard Blinn-Phong reflection model.");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneShaderCook::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addTextureToLoad(_texC,
                        AppDemo::texturePath +
                          "rusty-metal_2048_C.jpg");
    al.addTextureToLoad(_texN,
                        AppDemo::texturePath +
                          "rusty-metal_2048_N.jpg");
    al.addTextureToLoad(_texM,
                        AppDemo::texturePath +
                          "rusty-metal_2048_M.jpg");
    al.addTextureToLoad(_texR,
                        AppDemo::texturePath +
                          "rusty-metal_2048_R.jpg");
    al.addProgramToLoad(_sp,
                        AppDemo::shaderPath + "PerPixCook.vert",
                        AppDemo::shaderPath + "PerPixCook.frag");
    al.addProgramToLoad(_spTex,
                        AppDemo::shaderPath + "PerPixCookTm.vert",
                        AppDemo::shaderPath + "PerPixCookTm.frag");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneShaderCook::assemble(SLAssetManager* am, SLSceneView* sv)
{
    // Base root group node for the scene
    SLNode* scene = new SLNode;
    this->root3D(scene);

    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(0, 0, 30);
    cam1->lookAt(0, 0, 0);
    cam1->background().colors(SLCol4f::BLACK);
    cam1->focalDist(30);
    cam1->setInitialState();
    cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);
    scene->addChild(cam1);

    // Create spheres and materials with roughness & metallic values between 0 and 1
    const SLint nrRows  = 7;
    const SLint nrCols  = 7;
    SLfloat     spacing = 2.5f;
    SLfloat     maxX    = (float)(nrCols - 1) * spacing * 0.5f;
    SLfloat     maxY    = (float)(nrRows - 1) * spacing * 0.5f;
    SLfloat     deltaR  = 1.0f / (float)(nrRows - 1);
    SLfloat     deltaM  = 1.0f / (float)(nrCols - 1);

    SLMaterial* mat[nrRows * nrCols];
    SLint       i = 0;
    SLfloat     y = -maxY;
    for (SLint m = 0; m < nrRows; ++m)
    {
        SLfloat x = -maxX;
        for (SLint r = 0; r < nrCols; ++r)
        {
            if (m == nrRows / 2 && r == nrCols / 2)
            {
                // The center sphere has roughness and metallic encoded in textures
                mat[i] = new SLMaterial(am,
                                        "CookTorranceMatTex",
                                        nullptr,
                                        _texC,
                                        _texN,
                                        _texM,
                                        _texR,
                                        nullptr,
                                        _spTex);
            }
            else
            {
                // Cook-Torrance material without textures
                mat[i] = new SLMaterial(am,
                                        "CookTorranceMat",
                                        nullptr,
                                        SLCol4f::RED * 0.5f,
                                        Utils::clamp((float)r * deltaR, 0.05f, 1.0f),
                                        (float)m * deltaM,
                                        _sp);
            }

            SLNode* node = new SLNode(new SLSpheric(am, 1.0f, 0.0f, 180.0f, 32, 32, "Sphere", mat[i]));
            node->translate(x, y, 0);
            scene->addChild(node);
            x += spacing;
            i++;
        }
        y += spacing;
    }

    // Add 5 Lights: 2 point lights, 2 directional lights and 1 spotlight in the center.
    SLLight::gamma      = 2.2f;
    SLLightSpot* light1 = new SLLightSpot(am,
                                          this,
                                          -maxX,
                                          maxY,
                                          maxY,
                                          0.2f,
                                          180,
                                          0,
                                          1000,
                                          1000);
    light1->attenuation(0, 0, 1);
    SLLightDirect* light2 = new SLLightDirect(am,
                                              this,
                                              maxX,
                                              maxY,
                                              maxY,
                                              0.5f,
                                              0,
                                              10,
                                              10);
    light2->lookAt(0, 0, 0);
    light2->attenuation(0, 0, 1);
    SLLightSpot* light3 = new SLLightSpot(am,
                                          this,
                                          0,
                                          0,
                                          maxY,
                                          0.2f,
                                          36,
                                          0,
                                          1000,
                                          1000);
    light3->attenuation(0, 0, 1);
    SLLightDirect* light4 = new SLLightDirect(am,
                                              this,
                                              -maxX,
                                              -maxY,
                                              maxY,
                                              0.5f,
                                              0,
                                              10,
                                              10);
    light4->lookAt(0, 0, 0);
    light4->attenuation(0, 0, 1);
    SLLightSpot* light5 = new SLLightSpot(am,
                                          this,
                                          maxX,
                                          -maxY,
                                          maxY,
                                          0.2f,
                                          180,
                                          0,
                                          1000,
                                          1000);
    light5->attenuation(0, 0, 1);
    scene->addChild(light1);
    scene->addChild(light2);
    scene->addChild(light3);
    scene->addChild(light4);
    scene->addChild(light5);
    sv->camera(cam1);
}
//-----------------------------------------------------------------------------
