/**
 * \file      CVCalibration.h
 * \date      Winter 2016
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 * \authors   Michael Goettlicher, Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
*/

#ifndef CVCAMERA_H
#define CVCAMERA_H

#include <CVTypes.h>
#include <CVCalibration.h>

//-----------------------------------------------------------------------------
class CVCamera
{
public:
    CVCamera(CVCameraType type);

    bool         mirrorH() { return _mirrorH; }
    bool         mirrorV() { return _mirrorV; }
    CVCameraType type() { return _type; }
    void         showUndistorted(bool su) { _showUndistorted = su; }
    bool         showUndistorted() { return _showUndistorted; }
    int          camSizeIndex() { return _camSizeIndex; }

    void camSizeIndex(int index)
    {
        _camSizeIndex = index;
    }
    void toggleMirrorH() { _mirrorH = !_mirrorH; }
    void toggleMirrorV() { _mirrorV = !_mirrorV; }

    CVCalibration calibration;

private:
    bool         _showUndistorted = false; //!< Flag if image should be undistorted
    CVCameraType _type;
    bool         _mirrorH = false;
    bool         _mirrorV = false;

    int _camSizeIndex = -1;
};
//-----------------------------------------------------------------------------
#endif // CVCAMERA_H
