/**
 * \file      WebCamera.h
 * \brief     Interface to access the camera through the browser.
 * \date      October 2022
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLPROJECT_WEBCAMERA_H
#define SLPROJECT_WEBCAMERA_H

#include <CVTypedefs.h>

//-----------------------------------------------------------------------------
//! Facing modes for the camera
enum class WebCameraFacing
{
    FRONT = 0,
    BACK  = 1
};
//-----------------------------------------------------------------------------
//! Interface to access the camera in the browser
class WebCamera
{
public:
    void     open(WebCameraFacing facing);
    bool     isReady();
    CVMat    read();
    CVSize2i getSize();
    void     setSize(CVSize2i size);
    void     close();

    // Getters
    bool isOpened() { return _isOpened; }

private:
    bool  _isOpened = false;
    CVMat _image;
    CVMat _imageBGR;
    bool  _waitingForResize = false;
};
//-----------------------------------------------------------------------------
#endif // SLPROJECT_WEBCAMERA_H
