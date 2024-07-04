/**
 * \file      SLInterface.h
 * \brief     Declaration of the main Scene Library C-Interface.
 * \details   For more info on how to create a new app with SLProject see:
 *            https://github.com/cpvrlab/SLProject4/wiki/Creating-a-New-App
 *            For more info about App framework see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLINTERFACE_H
#define SLINTERFACE_H

#include <SLEnums.h>
#include <SLGLEnums.h>

class SLAssetManager;
class SLScene;
class SLSceneView;
class SLInputManager;

//-----------------------------------------------------------------------------
void slCreateApp(SLVstring&      cmdLineArgs,
                 const SLstring& dataPath,
                 const SLstring& shaderPath,
                 const SLstring& modelPath,
                 const SLstring& texturePath,
                 const SLstring& fontPath,
                 const SLstring& videoPath,
                 const SLstring& configPath,
                 const SLstring& applicationName);
//-----------------------------------------------------------------------------
SLint slCreateSceneView(SLAssetManager* am,
                        SLScene*        scene,
                        int             screenWidth,
                        int             screenHeight,
                        int             dotsPerInch,
                        SLSceneID       initScene,
                        void*           onWndUpdateCallback,
                        void*           onSelectNodeMeshCallback = nullptr,
                        void*           onNewSceneViewCallback   = nullptr,
                        void*           onImGuiBuild             = nullptr,
                        void*           onImGuiLoadConfig        = nullptr,
                        void*           onImGuiSaveConfig        = nullptr);
//-----------------------------------------------------------------------------
void slLoadCoreAssetsSync();
void slLoadCoreAssetsAsync();
void slSwitchScene(SLSceneView* sv, SLSceneID sceneID);
//-----------------------------------------------------------------------------
SLSceneView* slNewSceneView(SLScene* s, int dotsPerInch, SLInputManager& inputManager);
bool         slShouldClose();
void         slShouldClose(bool val);
void         slTerminate();
void         slResize(int sceneViewIndex, int width, int height);
bool         slUpdateParallelJob();
bool         slPaintAllViews();
//-----------------------------------------------------------------------------
void slMouseDown(int sceneViewIndex, SLMouseButton button, int x, int y, SLKey modifier);
void slMouseMove(int sceneViewIndex, int x, int y);
void slMouseUp(int sceneViewIndex, SLMouseButton button, int x, int y, SLKey modifier);
void slDoubleClick(int sceneViewIndex, SLMouseButton button, int x, int y, SLKey modifier);
void slTouch2Down(int sceneViewIndex, int x1, int y1, int x2, int y2);
void slTouch2Move(int sceneViewIndex, int x1, int y1, int x2, int y2);
void slTouch2Up(int sceneViewIndex, int x1, int y1, int x2, int y2);
void slTouch3Down(int sceneViewIndex, int x, int y);
void slTouch3Move(int sceneViewIndex, int x, int y);
void slTouch3Up(int sceneViewIndex, int x, int y);
void slMouseWheel(int sceneViewIndex, int pos, SLKey modifier);
void slKeyPress(int sceneViewIndex, SLKey key, SLKey modifier);
void slKeyRelease(int sceneViewIndex, SLKey key, SLKey modifier);
void slCharInput(int sceneViewIndex, unsigned int character);
//-----------------------------------------------------------------------------
bool   slUsesRotation();
void   slRotationQUAT(float quatX, float quatY, float quatZ, float quatW);
bool   slUsesLocation();
void   slLocationLatLonAlt(double latitudeDEG, double longitudeDEG, double altitudeM, float accuracyM);
string slGetWindowTitle(int sceneViewIndex);
void   slSetupExternalDir(const SLstring& externalDirPath);
void   slSetDeviceParameter(const SLstring& parameter, SLstring value);
//-----------------------------------------------------------------------------
#endif // SLINTERFACE_H
