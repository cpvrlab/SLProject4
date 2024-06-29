/**
 * \file      SLInputDevice.cpp
 * \date      January 2015
 * \authors   Marc Wacker, Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <algorithm>
#include <SLInputManager.h>

//-----------------------------------------------------------------------------
/*! Constructor for SLInputDevices. This will automatically enable the device,
adding them to the SLInputManager.
 */
SLInputDevice::SLInputDevice(SLInputManager& inputManager)
  : _inputManager(inputManager)
{
    // enable any input device on creation
    enable();
}
//-----------------------------------------------------------------------------
/*! The destructor removes the device from SLInputManager again if necessary.
 */
SLInputDevice::~SLInputDevice()
{
    disable();
}
//-----------------------------------------------------------------------------
/*! Enabling an SLInputDevice will add it to the device list kept by
SLInputManager
*/
void SLInputDevice::enable()
{
    _inputManager.devices().push_back(this);
}
//-----------------------------------------------------------------------------
/*! Enabling an SLInputDevice will remove it from the device list kept by
SLInputManager
*/
void SLInputDevice::disable()
{
    SLVInputDevice& dl = _inputManager.devices();
    dl.erase(std::remove(dl.begin(), dl.end(), this), dl.end());
}
//-----------------------------------------------------------------------------
