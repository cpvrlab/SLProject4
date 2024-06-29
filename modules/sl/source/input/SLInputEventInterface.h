/**
 * \file      SLInputEventInterface.h
 * \date      January 2015
 * \authors   Marc Wacker, Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SL_INPUTEVENTINTERFACE_H
#define SL_INPUTEVENTINTERFACE_H

#include <string>
#include <SLEnums.h>

//-----------------------------------------------------------------------------
class SLInputManager;
//-----------------------------------------------------------------------------
class SLInputEventInterface
{
public:
    SLInputEventInterface(SLInputManager& inputManager);

    void resize(int sceneViewIndex, int width, int height);
    void updateScr2fb(int sceneViewIndex);

    void mouseDown(int sceneViewIndex, SLMouseButton button, int x, int y, SLKey modifier);
    void mouseMove(int sceneViewIndex, int x, int y);
    void mouseUp(int sceneViewIndex, SLMouseButton button, int x, int y, SLKey modifier);
    void doubleClick(int sceneViewIndex, SLMouseButton button, int x, int y, SLKey modifier);
    void touch2Down(int sceneViewIndex, int x1, int y1, int x2, int y2);
    void touch2Move(int sceneViewIndex, int x1, int y1, int x2, int y2);
    void touch2Up(int sceneViewIndex, int x1, int y1, int x2, int y2);
    void mouseWheel(int sceneViewIndex, int pos, SLKey modifier);
    void keyPress(int sceneViewIndex, SLKey key, SLKey modifier);
    void keyRelease(int sceneViewIndex, SLKey key, SLKey modifier);
    void charInput(int sceneViewIndex, unsigned int character);
    void longTouch(int sceneViewIndex, int x, int y) {}
    void scrCaptureRequest(int sceneViewIndex, std::string outputPath);

private:
    SLInputManager& _inputManager;
};
//-----------------------------------------------------------------------------
#endif
