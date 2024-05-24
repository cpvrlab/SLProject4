//#############################################################################
//  File:      AppDemoSceneShaderIBL.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneShaderIBL.h>

#include <SLLightSpot.h>
#include <SLLightDirect.h>
#include <SLSphere.h>
#include <AppDemo.h>

//-----------------------------------------------------------------------------
AppDemoSceneShaderIBL::AppDemoSceneShaderIBL()
  : AppScene("Image Based Lighting Test Scene")
{
    info("Image-based lighting from skybox using high dynamic range images. "
         "It uses the Cook-Torrance reflection model also to calculate the "
         "ambient light part from the surrounding HDR skybox.");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneShaderIBL::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addTextureToLoad(_texC,
                        AppDemo::texturePath +
                          "gold-scuffed_2048_C.png");
    al.addTextureToLoad(_texN,
                        AppDemo::texturePath +
                          "gold-scuffed_2048_N.png");
    al.addTextureToLoad(_texM,
                        AppDemo::texturePath +
                          "gold-scuffed_2048_M.png");
    al.addTextureToLoad(_texR,
                        AppDemo::texturePath +
                          "gold-scuffed_2048_R.png");
    al.addTextureToLoad(_texA,
                        AppDemo::texturePath +
                          "gold-scuffed_2048_A.png");

    al.addSkyboxToLoad(_skybox,
                       al.texturePath() + "env_barce_rooftop.hdr",
                       SLVec2i(256, 256),
                       "HDR Skybox");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneShaderIBL::assemble(SLAssetManager* am, SLSceneView* sv)
{
    // Create a scene group node
    SLNode* scene = new SLNode("scene node");
    this->root3D(scene);

    // Create camera and initialize its parameters
    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(0, 0, 30);
    cam1->lookAt(0, 0, 0);
    cam1->background().colors(SLCol4f(0.2f, 0.2f, 0.2f));
    cam1->focalDist(30);
    cam1->setInitialState();
    scene->addChild(cam1);

    // Add directional light with a position that corresponds roughly to the sun direction
    SLLight::gamma        = 2.2f;
    SLLightDirect* light1 = new SLLightDirect(am,
                                              this,
                                              4.0f,
                                              .3f,
                                              2.0f,
                                              0.5f,
                                              0,
                                              1,
                                              1);
    light1->lookAt(0, 0, 0);
    light1->attenuation(1, 0, 0);
    light1->createsShadows(true);
    light1->createShadowMapAutoSize(cam1, SLVec2i(2048, 2048), 4);
    light1->shadowMap()->cascadesFactor(30.0);
    light1->doSmoothShadows(true);
    light1->castsShadows(false);
    light1->shadowMinBias(0.001f);
    light1->shadowMaxBias(0.003f);
    scene->addChild(light1);

    // Create spheres and materials with roughness & metallic values between 0 and 1
    const SLint nrRows  = 7;
    const SLint nrCols  = 7;
    SLfloat     spacing = 2.5f;
    SLfloat     maxX    = (float)((int)(nrCols / 2) * spacing);
    SLfloat     maxY    = (float)((int)(nrRows / 2) * spacing);
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
                // and the prefiltered textures for IBL
                mat[i] = new SLMaterial(am,
                                        "IBLMatTex",
                                        _skybox,
                                        _texC,
                                        _texN,
                                        _texM,
                                        _texR,
                                        _texA);
            }
            else
            {
                // Cook-Torrance material with IBL but without textures
                mat[i] = new SLMaterial(am,
                                        "IBLMat",
                                        _skybox,
                                        SLCol4f::WHITE * 0.5f,
                                        Utils::clamp((float)r * deltaR, 0.05f, 1.0f),
                                        (float)m * deltaM);
            }

            SLNode* node = new SLNode(new SLSpheric(am,
                                                    1.0f,
                                                    0.0f,
                                                    180.0f,
                                                    32,
                                                    32,
                                                    "Sphere",
                                                    mat[i]));
            node->translate(x, y, 0);
            scene->addChild(node);
            x += spacing;
            i++;
        }
        y += spacing;
    }

    sv->camera(cam1);
    this->skybox(_skybox);

    // Save energy
    sv->doWaitOnIdle(true);
}
//-----------------------------------------------------------------------------
