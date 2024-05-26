//#############################################################################
//  File:      AppDemoSceneLevelOfDetail.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marcus Hudritsch
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneLevelOfDetail.h>
#include <AppDemo.h>
#include <SLAssetLoader.h>
#include <SLRectangle.h>
#include <SLNodeLOD.h>

//-----------------------------------------------------------------------------
AppDemoSceneLevelOfDetail::AppDemoSceneLevelOfDetail(SLSceneID sceneID)
  : SLScene("Level of Detail Test"),
    _sceneID(sceneID)
{
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneLevelOfDetail::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addTextureToLoad(_texFloorDif,
                        AppDemo::modelPath +
                          "GLTF/CorinthianColumn/PavementSlateSquare2_2K_DIF.jpg",
                        SL_ANISOTROPY_MAX);
    al.addTextureToLoad(_texFloorNrm,
                        AppDemo::modelPath +
                          "GLTF/CorinthianColumn/PavementSlateSquare2_2K_NRM.jpg",
                        SL_ANISOTROPY_MAX);
    al.addNodeToLoad(_columnLOD,
                     AppDemo::modelPath +
                       "GLTF/CorinthianColumn/Corinthian-Column-Round-LOD.gltf");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneLevelOfDetail::assemble(SLAssetManager* am, SLSceneView* sv)
{
    SLchar name[512];
    SLint  size;
    if (_sceneID == SID_Benchmark_ColumnsNoLOD)
    {
        size = 25;
        snprintf(name,
                 sizeof(name),
                 "%d corinthian columns without LOD",
                 size * size);
        this->name(name);
    }
    else
    {
        size = 50;
        snprintf(name,
                 sizeof(name),
                 "%d corinthian columns with LOD",
                 size * size);
        this->name(name);
    }
    info(this->name() + " with cascaded shadow mapping. In the Day-Time dialogue you can change the sun angle.");

    // Create ground material
    SLMaterial* matFloor = new SLMaterial(am,
                                          "matFloor",
                                          _texFloorDif,
                                          _texFloorNrm);

    // Define camera
    SLCamera* cam1 = new SLCamera;
    cam1->translation(0, 1.7f, 20);
    cam1->lookAt(0, 1.7f, 0);
    cam1->focalDist(cam1->translationOS().length());
    cam1->clipFar(600);
    cam1->background().colors(SLCol4f(0.1f, 0.4f, 0.8f));
    cam1->setInitialState();

    // Create directional light for the sunlight
    SLLightDirect* sunLight = new SLLightDirect(am, this, 1.0f);
    sunLight->powers(0.25f, 1.0f, 1.0f);
    sunLight->attenuation(1, 0, 0);
    sunLight->translation(0, 1.7f, 0);
    sunLight->lookAt(-1, 0, -1);
    sunLight->doSunPowerAdaptation(true);

    // Add cascaded shadow mapping
    sunLight->createsShadows(true);
    sunLight->createShadowMapAutoSize(cam1);
    sunLight->doSmoothShadows(true);
    sunLight->castsShadows(false);
    sunLight->shadowMinBias(0.003f);
    sunLight->shadowMaxBias(0.012f);

    // Let the sun be rotated by time and location
    AppDemo::devLoc.sunLightNode(sunLight);
    AppDemo::devLoc.originLatLonAlt(47.14271,
                                    7.24337,
                                    488.2); // Ecke Giosa
    AppDemo::devLoc.defaultLatLonAlt(47.14260,
                                     7.24310,
                                     488.7 + 1.7); // auf Parkplatz

    // Floor rectangle
    SLNode* rect = new SLNode(new SLRectangle(am,
                                              SLVec2f(-200, -200),
                                              SLVec2f(200, 200),
                                              SLVec2f(0, 0),
                                              SLVec2f(50, 50),
                                              50,
                                              50,
                                              "Floor",
                                              matFloor));
    rect->rotate(90, -1, 0, 0);
    rect->castsShadows(false);

    // Configure the corinthian column
    SLNode* columnL0 = _columnLOD->findChild<SLNode>("Corinthian-Column-Round-L0");
    SLNode* columnL1 = _columnLOD->findChild<SLNode>("Corinthian-Column-Round-L1");
    SLNode* columnL2 = _columnLOD->findChild<SLNode>("Corinthian-Column-Round-L2");
    SLNode* columnL3 = _columnLOD->findChild<SLNode>("Corinthian-Column-Round-L3");

    // Assemble scene
    SLNode* scene = new SLNode("Scene");
    root3D(scene);
    scene->addChild(sunLight);
    scene->addChild(rect);
    scene->addChild(cam1);

    // create loads of pillars
    SLint   numColumns = size * size;
    SLfloat offset     = 5.0f;
    SLfloat z          = (float)(size - 1) * offset * 0.5f;

    for (SLint iZ = 0; iZ < size; ++iZ)
    {
        SLfloat x = -(float)(size - 1) * offset * 0.5f;

        for (SLint iX = 0; iX < size; ++iX)
        {
            SLint iZX = iZ * size + iX;

            if (_sceneID == SID_Benchmark_ColumnsNoLOD)
            {
                // Without just the level 0 node
                string  strNode = "Node" + std::to_string(iZX);
                SLNode* column  = new SLNode(columnL1->mesh(),
                                            strNode + "-L0");
                column->translate(x, 0, z, TS_object);
                scene->addChild(column);
            }
            else
            {
                // With LOD parent node and 3 levels
                string     strLOD    = "LOD" + std::to_string(iZX);
                SLNodeLOD* lod_group = new SLNodeLOD(strLOD);
                lod_group->translate(x, 0, z, TS_object);
                lod_group->addChildLOD(new SLNode(columnL1->mesh(),
                                                  strLOD + "-L0"),
                                       0.1f,
                                       3);
                lod_group->addChildLOD(new SLNode(columnL2->mesh(),
                                                  strLOD + "-L1"),
                                       0.01f,
                                       3);
                lod_group->addChildLOD(new SLNode(columnL3->mesh(),
                                                  strLOD + "-L2"),
                                       0.0001f,
                                       3);
                scene->addChild(lod_group);
            }
            x += offset;
        }
        z -= offset;
    }

    // Set active camera & the root pointer
    sv->camera(cam1);
    sv->doWaitOnIdle(false);
}
//-----------------------------------------------------------------------------
