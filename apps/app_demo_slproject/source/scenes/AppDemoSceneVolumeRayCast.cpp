//#############################################################################
//  File:      AppDemoSceneVolumeRayCast.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneVolumeRayCast.h>
#include <SLLightSpot.h>
#include <SLBox.h>
#include <AppDemo.h>

//-----------------------------------------------------------------------------
AppDemoSceneVolumeRayCast::AppDemoSceneVolumeRayCast()
  : AppScene("Volume Ray Cast Test")
{
    info("Volume Rendering of an angiographic MRI scan");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneVolumeRayCast::registerAssetsToLoad(SLAssetLoader& al)
{
    // Load volume data into 3D texture
    SLVstring mriImages;
    for (SLint i = 0; i < 207; ++i)
        mriImages.push_back(Utils::formatString(al.texturePath() + "i%04u_0000b.png", i));

    SLint clamping3D = GL_CLAMP_TO_EDGE;
    if (SLGLState::instance()->getSLVersionNO() > "320")
        clamping3D = 0x812D; // GL_CLAMP_TO_BORDER

    al.addTextureToLoad(_mriTex3D,
                        mriImages,
                        GL_LINEAR,
                        GL_LINEAR,
                        clamping3D,
                        clamping3D,
                        "mri_head_front_to_back",
                        false);
    al.addProgramToLoad(_sp,
                        AppDemo::shaderPath + "VolumeRenderingRayCast.vert",
                        AppDemo::shaderPath + "VolumeRenderingRayCast.frag");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneVolumeRayCast::assemble(SLAssetManager* am, SLSceneView* sv)
{
    // Create transfer LUT 1D texture
    SLVAlphaLUTPoint tfAlphas = {SLAlphaLUTPoint(0.00f, 0.00f),
                                 SLAlphaLUTPoint(0.01f, 0.75f),
                                 SLAlphaLUTPoint(1.00f, 1.00f)};
    SLTexColorLUT*   tf       = new SLTexColorLUT(am,
                                          tfAlphas,
                                          CLUT_BCGYR);

    // Load shader and uniforms for volume size
    SLGLUniform1f* volX = new SLGLUniform1f(UT_const,
                                            "u_volumeX",
                                            (SLfloat)_mriTex3D->images()[0]->width());
    SLGLUniform1f* volY = new SLGLUniform1f(UT_const,
                                            "u_volumeY",
                                            (SLfloat)_mriTex3D->images()[0]->height());
    SLGLUniform1f* volZ = new SLGLUniform1f(UT_const,
                                            "u_volumeZ",
                                            (SLfloat)_mriTex3D->images().size());
    _sp->addUniform1f(volX);
    _sp->addUniform1f(volY);
    _sp->addUniform1f(volZ);

    // Create volume rendering material
    SLMaterial* matVR = new SLMaterial(am,
                                       "matVR",
                                       _mriTex3D,
                                       tf,
                                       nullptr,
                                       nullptr,
                                       _sp);

    // Create camera
    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(0, 0, 3);
    cam1->lookAt(0, 0, 0);
    cam1->focalDist(3);
    cam1->background().colors(SLCol4f(0, 0, 0));
    cam1->setInitialState();
    cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);

    // Set light
    SLLightSpot* light1 = new SLLightSpot(am, this, 0.3f);
    light1->powers(0.1f, 1.0f, 1.0f);
    light1->attenuation(1, 0, 0);
    light1->translation(5, 5, 5);

    // Assemble scene with box node
    SLNode* scene = new SLNode("Scene");
    root3D(scene);
    scene->addChild(light1);
    scene->addChild(new SLNode(new SLBox(am,
                                         -1,
                                         -1,
                                         -1,
                                         1,
                                         1,
                                         1,
                                         "Box",
                                         matVR)));
    scene->addChild(cam1);

    sv->camera(cam1);
}
//-----------------------------------------------------------------------------
