/**
 * \file      SLInputDevice.h
 * \date      January 2015
 * \authors   Marc Wacker, Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLINPUTDEVICE_H
#define SLINPUTDEVICE_H

#include <SL.h>
#include <vector>

class SLInputManager;

//-----------------------------------------------------------------------------
//! Interface for input devices that have to be pollsed
class SLInputDevice
{
public:
    explicit SLInputDevice(SLInputManager& inputManager);
    virtual ~SLInputDevice();

    void enable();
    void disable();

    /** Polls a custom input device. returns true if the poll resulted in
    event's being sent out that were accepted by some receiver. */
    virtual SLbool poll() = 0;

private:
    SLInputManager& _inputManager;
};
//-----------------------------------------------------------------------------
typedef vector<SLInputDevice*> SLVInputDevice;
//-----------------------------------------------------------------------------
#endif
