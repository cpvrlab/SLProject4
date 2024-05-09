//#############################################################################
//  File:      AppDemoSceneFrustum.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneFrustum.h>
#include <SLLightSpot.h>
#include <SLSphere.h>
#include <AppDemo.h>

//-----------------------------------------------------------------------------
AppDemoSceneFrustum::AppDemoSceneFrustum() : AppScene("Frustum Culling Test Scene")
{
    info("View frustum culling: Only objects in view are rendered. "
         "You can turn view frustum culling on/off in the menu Preferences or with the key F.");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneFrustum::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addTextureToLoad(_tex, "earth1024_C.jpg");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneFrustum::assemble(SLAssetManager* am, SLSceneView* sv)
{
    // create texture
    SLMaterial* mat1 = new SLMaterial(am, "mat1", _tex);

    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->clipNear(0.1f);
    cam1->clipFar(100);
    cam1->translation(0, 0, 1);
    cam1->lookAt(0, 0, 0);
    cam1->focalDist(5);
    cam1->background().colors(SLCol4f(0.1f, 0.1f, 0.1f));
    cam1->setInitialState();
    cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);

    SLLightSpot* light1 = new SLLightSpot(am,
                                          this,
                                          10,
                                          10,
                                          10,
                                          0.3f);
    light1->powers(0.2f, 0.8f, 1.0f);
    light1->attenuation(1, 0, 0);

    SLNode* scene = new SLNode;
    this->root3D(scene);
    scene->addChild(cam1);
    scene->addChild(light1);

    // add one single sphere in the center
    SLuint  res    = 16;
    SLNode* sphere = new SLNode(new SLSphere(am,
                                             0.15f,
                                             res,
                                             res,
                                             "mySphere",
                                             mat1));
    scene->addChild(sphere);

    // create spheres around the center sphere
    SLint size = 20;
    for (SLint iZ = -size; iZ <= size; ++iZ)
    {
        for (SLint iY = -size; iY <= size; ++iY)
        {
            for (SLint iX = -size; iX <= size; ++iX)
            {
                if (iX != 0 || iY != 0 || iZ != 0)
                {
                    SLNode* sph = sphere->copyRec();
                    sph->translate(float(iX),
                                   float(iY),
                                   float(iZ),
                                   TS_object);
                    scene->addChild(sph);
                }
            }
        }
    }

    SLuint num = (SLuint)(size + size + 1);
    SL_LOG("Triangles on GPU: %u", res * res * 2 * num * num * num);

    sv->camera(cam1);
    sv->doWaitOnIdle(false);
}
//-----------------------------------------------------------------------------
