/**
 * \file      AppDemoSceneAnimNodeMass.cpp
 * \brief     Implementation for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <AppDemoSceneAnimNodeMass.h>
#include <AppCommon.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>
#include <SLBox.h>

//-----------------------------------------------------------------------------
AppDemoSceneAnimNodeMass::AppDemoSceneAnimNodeMass()
  : SLScene("Mass Animation Test Scene")
{
    info("Performance test for transform updates from many animations.");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneAnimNodeMass::registerAssetsToLoad(SLAssetLoader& al)
{
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneAnimNodeMass::assemble(SLAssetManager* am, SLSceneView* sv)
{
    // Create a scene group node
    SLNode* scene = new SLNode("scene node");
    root3D(scene);

    // Create and add camera
    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(0, 20, 40);
    cam1->lookAt(0, 0, 0);
    cam1->focalDist(42);
    scene->addChild(cam1);
    sv->camera(cam1);

    // Add spotlight
    SLLightSpot* light1 = new SLLightSpot(am, this, 0.1f);
    light1->translate(0, 10, 0);
    scene->addChild(light1);

    // build a basic scene to have a reference for the occurring rotations
    SLMaterial* genericMat = new SLMaterial(am, "some material");

    // we use the same mesh to visualize all the nodes
    SLBox* box = new SLBox(am,
                           -0.5f,
                           -0.5f,
                           -0.5f,
                           0.5f,
                           0.5f,
                           0.5f,
                           "box",
                           genericMat);

    // We build a stack of levels, each level has a grid of boxes on it
    // each box on this grid has another grid above it with child nodes.
    // Best results are achieved if gridSize is an uneven number.
    // (gridSize^2)^levels = num nodes. handle with care.
    const SLint levels      = 3;
    const SLint gridSize    = 3;
    const SLint gridHalf    = gridSize / 2;
    const SLint nodesPerLvl = gridSize * gridSize;

    // node spacing per level
    // nodes are 1^3 in size, we want to space the levels so that the densest levels meet
    // (so exactly 1 unit spacing between blocks)
    SLfloat nodeSpacing[levels];
    for (SLint i = 0; i < levels; ++i)
        nodeSpacing[(levels - 1) - i] = (SLfloat)pow((SLfloat)gridSize, (SLfloat)i);

    // lists to keep track of previous grid level to set parents correctly
    vector<SLNode*> parents;
    vector<SLNode*> curParentsVector;

    // first parent is the scene root
    parents.push_back(scene);

    SLint nodeIndex = 0;
    for (float lvl : nodeSpacing)
    {
        curParentsVector = parents;
        parents.clear();

        // for each parent in the previous level, add a completely new grid
        for (auto parent : curParentsVector)
        {
            for (SLint i = 0; i < nodesPerLvl; ++i)
            {
                SLNode* node = new SLNode("MassAnimNode");
                node->addMesh(box);
                parent->addChild(node);
                parents.push_back(node);

                // position
                SLfloat x = (SLfloat)(i % gridSize - gridHalf);
                SLfloat z = (SLfloat)((i > 0) ? i / gridSize - gridHalf : -gridHalf);
                SLVec3f pos(x * lvl * 1.1f, 1.5f, z * lvl * 1.1f);

                node->translate(pos, TS_object);
                // node->scale(1.1f);

                SLfloat       duration = 1.0f + 5.0f * ((SLfloat)i / (SLfloat)nodesPerLvl);
                ostringstream oss;

                oss << "random anim " << nodeIndex++;
                SLAnimation* anim = animManager().createNodeAnimation(oss.str(),
                                                                      duration,
                                                                      true,
                                                                      EC_inOutSine,
                                                                      AL_pingPongLoop);
                anim->createNodeAnimTrackForTranslation(node,
                                                        SLVec3f(0.0f, 1.0f, 0.0f));
            }
        }
    }
}
//-----------------------------------------------------------------------------
