/**
 * \file      AppMinimal.cpp
 * \brief     Single file minimal full app with SLProject
 * \details   For more info on how to create a new app with SLProject see:
 *            https://github.com/cpvrlab/SLProject4/wiki/Creating-a-New-App
 *            For more info about App framework see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      June 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <App.h>
#include <SLScene.h>
#include <SLBox.h>
#include <SLLightDirect.h>
#include <SLSceneView.h>
#include <SLAssetLoader.h>
#include <SLImGui.h>
#include <SLInterface.h>
#include <AppDemo.h>

//-----------------------------------------------------------------------------
//! Scene class derived from SLScene
class AppMinimalScene : public SLScene
{
public:
    AppMinimalScene();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLGLTexture* _woodTexture;
};
//-----------------------------------------------------------------------------
AppMinimalScene::AppMinimalScene() : SLScene("Minimal Demo Scene")
{
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppMinimalScene::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addTextureToLoad(_woodTexture,
                        AppDemo::texturePath + "wood0_0512_C.jpg");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppMinimalScene::assemble(SLAssetManager* am, SLSceneView* sv)
{
    SLCamera* camera = new SLCamera();
    camera->translation(2, 1, 3);
    camera->lookAt(0, 0, 0);
    camera->focalDist(camera->translationWS().distance(SLVec3f::ZERO));

    SLLightDirect* light = new SLLightDirect(am, this);
    light->translate(-0.5f, 1.0f, 0.75f);
    light->lookAt(0, 0, 0);

    SLMaterial* material = new SLMaterial(am,
                                          "Wood Material",
                                          _woodTexture);
    SLNode*     box      = new SLNode("Box Node");
    box->addMesh(new SLBox(am,
                           -0.5f,
                           -0.5f,
                           -0.5f,
                           0.5f,
                           0.5f,
                           0.5f,
                           "Box",
                           material));

    SLNode* scene = new SLNode();
    scene->addChild(light);
    scene->addChild(camera);
    scene->addChild(box);
    root3D(scene);

    sv->camera(camera);
    sv->doWaitOnIdle(true);
}
//-----------------------------------------------------------------------------
//! A static function in which you can create the new scene
static SLScene* createScene(SLSceneID sceneID)
{
    return new AppMinimalScene();
}
//-----------------------------------------------------------------------------
//! A static function that builds the UI with ImGui
static void buildGui(SLScene* s, SLSceneView* sv)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Quit"))
                slShouldClose(true);

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}
//-----------------------------------------------------------------------------
int SL_MAIN_FUNCTION(int argc, char* argv[])
{
    App::Config config;
    config.argc         = argc,
    config.argv         = argv;
    config.windowWidth  = 640;
    config.windowHeight = 480;
    config.windowTitle  = "SLProject Minimal App";
    config.onNewScene   = createScene;
    config.onGuiBuild   = buildGui;

    return App::run(config);
}
//-----------------------------------------------------------------------------
