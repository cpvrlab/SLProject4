//#############################################################################
//  File:      AppMinimal.cpp
//  Date:      June 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <App.h>
#include <SLScene.h>
#include <SLBox.h>
#include <SLLightDirect.h>
#include <SLSceneView.h>
#include <SLAssetLoader.h>
#include <SLGLImGui.h>
#include <SLInterface.h>
#include <AppDemo.h>

//-----------------------------------------------------------------------------
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
AppMinimalScene::AppMinimalScene()
  : SLScene("Minimal Demo Scene")
{
}
//-----------------------------------------------------------------------------
void AppMinimalScene::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addTextureToLoad(_woodTexture,
                        AppDemo::texturePath + "wood0_0512_C.jpg");
}
//-----------------------------------------------------------------------------
void AppMinimalScene::assemble(SLAssetManager* am, SLSceneView* sv)
{
    SLCamera* camera = new SLCamera();
    camera->translation(2, 1, 3);
    camera->lookAt(0, 0, 0);
    camera->focalDist(camera->translationWS().distance(SLVec3f::ZERO));

    SLLightDirect* light = new SLLightDirect(am, this);
    light->translate(-0.5f, 1.0f, 0.75f);
    light->lookAt(0, 0, 0);

    SLMaterial* material = new SLMaterial(am, "Wood Material", _woodTexture);
    SLNode*     box      = new SLNode("Box Node");
    box->addMesh(new SLBox(am, -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, "Box", material));

    SLNode* scene = new SLNode();
    scene->addChild(light);
    scene->addChild(camera);
    scene->addChild(box);
    root3D(scene);

    sv->camera(camera);
    sv->doWaitOnIdle(false);
}
//-----------------------------------------------------------------------------
static SLScene* createScene(SLSceneID sceneID)
{
    return new AppMinimalScene();
}
//-----------------------------------------------------------------------------
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
    return App::run({.argc         = argc,
                     .argv         = argv,
                     .windowTitle  = "SLProject Minimal App",
                     .windowWidth  = 640,
                     .windowHeight = 480,
                     .onNewScene   = createScene,
                     .onGuiBuild   = buildGui});
}
//-----------------------------------------------------------------------------
