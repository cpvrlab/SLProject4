/**
 * \file      SLHorizonNode.h
 * \brief     Scene node that draws the device's horizon line and tilt angle
 * \date      November 2020
 * \authors   Michael Göttlicher, Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 */

#ifndef SL_HORIZON_NODE_H
#define SL_HORIZON_NODE_H

#include <SLNode.h>
#include <SLTexFont.h>
#include <SLDeviceRotation.h>
#include <SLPolyline.h>
#include <SLGLProgramGeneric.h>
#include <SLMaterial.h>

//-----------------------------------------------------------------------------
//! Scene node that visualises the horizon estimated from the device rotation
/*! The node owns two children: a polyline drawn as the horizon itself and an
SLText showing the roll angle in degrees. Every frame doUpdate() reads the
averaged rotation from the SLDeviceRotation passed to the constructor, derives
the horizon direction with SLAlgo::estimateHorizon and rotates the line to
match. The displayed angle is negated, so it expresses the rotation of the
device with respect to the horizon rather than the other way round.

Intended for AR scenes on mobile devices, where it gives the user a reference
for how far the device is tilted. */
class SLHorizonNode : public SLNode
{
public:
    //! Builds the horizon line and its text label
    /*! \param name      Name of the node
        \param devRot    Rotation sensor to read the orientation from, not owned
        \param font      Font for the angle label; if nullptr no label is shown
        \param shaderDir Directory the line shader is loaded from
        \param scrW      Screen width in pixels, used to size the line
        \param scrH      Screen height in pixels, used to size the line */
    SLHorizonNode(SLstring          name,
                  SLDeviceRotation* devRot,
                  SLTexFont*        font,
                  SLstring          shaderDir,
                  int               scrW,
                  int               scrH);

    //! Deletes the program, material and meshes created in the constructor
    ~SLHorizonNode();

    //! Re-aligns the horizon line and refreshes the angle label
    /*! Called once per frame by the scene update. The text node is destroyed
    and rebuilt on every call because the label text changes. */
    void doUpdate() override;

private:
    SLDeviceRotation* _devRot = nullptr; //!< Rotation sensor, not owned
    SLTexFont*        _font   = nullptr; //!< Font of the angle label, not owned
    SLstring          _shaderDir;        //!< Directory of the line shader

    SLGLProgram* _prog        = nullptr; //!< Program drawing the horizon line
    SLMaterial*  _mat         = nullptr; //!< Material of the horizon line
    SLPolyline*  _line        = nullptr; //!< Mesh of the horizon line
    SLNode*      _horizonNode = nullptr; //!< Child node carrying the line
    SLNode*      _textNode    = nullptr; //!< Child node carrying the angle text

    SLMat3f _sRc; //!< Sensor to camera rotation used by the horizon estimation
};
//-----------------------------------------------------------------------------
#endif
