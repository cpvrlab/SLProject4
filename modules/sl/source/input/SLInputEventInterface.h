/**
 * \file      SLInputEventInterface.h
 * \brief     Convenience interface for posting input events to SLInputManager
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
//! Convenience interface for posting input events to an SLInputManager
/*! The platform layer receives native events (GLFW, JNI, Objective-C or
JavaScript) and forwards them through this interface, which packages each one
as an SLInputEvent and queues it on the SLInputManager. The queue is drained
during the scene update, so events are never handled on the caller's stack.

Every method takes the index of the SLSceneView the event belongs to, which is
carried through to the handler so that multiple views can be fed independently. */
class SLInputEventInterface
{
public:
    //! \param inputManager Manager the events are queued on; must outlive this object
    SLInputEventInterface(SLInputManager& inputManager);

    //! Posts a resize of the scene view to the given width and height in pixels
    void resize(int sceneViewIndex, int width, int height);

    //! Posts a request to recompute the screen to framebuffer scale factor
    /*! Needed on high DPI displays, where the framebuffer is larger than the
    window in screen coordinates. */
    void updateScr2fb(int sceneViewIndex);

    //! Posts a mouse button press at position x, y
    void mouseDown(int sceneViewIndex, SLMouseButton button, int x, int y, SLKey modifier);

    //! Posts a mouse movement to position x, y
    void mouseMove(int sceneViewIndex, int x, int y);

    //! Posts a mouse button release at position x, y
    void mouseUp(int sceneViewIndex, SLMouseButton button, int x, int y, SLKey modifier);

    //! Posts a mouse button double click at position x, y
    void doubleClick(int sceneViewIndex, SLMouseButton button, int x, int y, SLKey modifier);

    //! Posts a two finger touch down at the two positions x1, y1 and x2, y2
    void touch2Down(int sceneViewIndex, int x1, int y1, int x2, int y2);

    //! Posts a two finger move to the two positions x1, y1 and x2, y2
    void touch2Move(int sceneViewIndex, int x1, int y1, int x2, int y2);

    //! Posts a two finger lift off from the two positions x1, y1 and x2, y2
    void touch2Up(int sceneViewIndex, int x1, int y1, int x2, int y2);

    //! Posts a mouse wheel movement by pos detents
    void mouseWheel(int sceneViewIndex, int pos, SLKey modifier);

    //! Posts a key press, for keys that act as commands
    void keyPress(int sceneViewIndex, SLKey key, SLKey modifier);

    //! Posts a key release
    void keyRelease(int sceneViewIndex, SLKey key, SLKey modifier);

    //! Posts a translated character, for text entry rather than commands
    void charInput(int sceneViewIndex, unsigned int character);

    //! Long touch gesture; accepted but currently ignored
    void longTouch(int sceneViewIndex, int x, int y) {}

    //! Posts a request to write a screenshot of the scene view to outputPath
    void scrCaptureRequest(int sceneViewIndex, std::string outputPath);

private:
    SLInputManager& _inputManager; //!< Manager the events are queued on
};
//-----------------------------------------------------------------------------
#endif
