//#############################################################################
//  File:      AppDemoSceneLotsOfNodes.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneLotsOfNodes.h>
#include <AppDemo.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>
#include <SLSphere.h>

//-----------------------------------------------------------------------------
AppDemoSceneLotsOfNodes::AppDemoSceneLotsOfNodes()
  : SLScene("Lots of Nodes Benchmark Scene")
{
    info("Lots of Nodes Benchmark Scene");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneLotsOfNodes::registerAssetsToLoad(SLAssetLoader& al)
{
    for (int i = 0; i < _NUM_MAT; ++i)
    {
        al.addTextureToLoad(_texC[i],
                            AppDemo::texturePath + "earth2048_C_Q95.jpg");
    }
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneLotsOfNodes::assemble(SLAssetManager* am, SLSceneView* sv)
{
    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->clipNear(0.1f);
    cam1->clipFar(100);
    cam1->translation(0, 0, 50);
    cam1->lookAt(0, 0, 0);
    cam1->focalDist(50);
    cam1->background().colors(SLCol4f(0.1f, 0.1f, 0.1f));
    cam1->setInitialState();

    SLLightSpot* light1 = new SLLightSpot(am,
                                          this,
                                          15,
                                          15,
                                          15,
                                          0.3f);
    light1->powers(0.2f, 0.8f, 1.0f);
    light1->attenuation(1, 0, 0);

    SLNode* scene = new SLNode;
    root3D(scene);
    scene->addChild(cam1);
    scene->addChild(light1);

    // Generate NUM_MAT materials
    SLVMaterial mat;
    for (int i = 0; i < _NUM_MAT; ++i)
    {
        SLstring matName = "mat-" + std::to_string(i);
        mat.push_back(new SLMaterial(am, matName.c_str(), _texC[i]));
        SLCol4f color;
        color.hsva2rgba(SLVec4f(Utils::TWOPI * (float)i / (float)_NUM_MAT,
                                1.0f,
                                1.0f));
        mat[i]->diffuse(color);
    }

    // create a 3D array of spheres
    SLint  halfSize = 10;
    SLuint n        = 0;
    for (SLint iZ = -halfSize; iZ <= halfSize; ++iZ)
    {
        for (SLint iY = -halfSize; iY <= halfSize; ++iY)
        {
            for (SLint iX = -halfSize; iX <= halfSize; ++iX)
            {
                // Choose a random material index
                SLuint   res      = 36;
                SLint    iMat     = (SLint)Utils::random(0, _NUM_MAT - 1);
                SLstring nodeName = "earth-" + std::to_string(n);

                // Create a new sphere and node and translate it
                SLSphere* earth  = new SLSphere(am,
                                               0.3f,
                                               res,
                                               res,
                                               nodeName,
                                               mat[iMat]);
                SLNode*   sphere = new SLNode(earth);
                sphere->translate(float(iX),
                                  float(iY),
                                  float(iZ),
                                  TS_object);
                scene->addChild(sphere);
                n++;
            }
        }
    }

    sv->camera(cam1);
    sv->doWaitOnIdle(false);
}
//-----------------------------------------------------------------------------
