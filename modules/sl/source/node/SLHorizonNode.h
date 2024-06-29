/**
 * \file      SLHorizonNode.h
 * \authors   Michael Göttlicher
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
class SLHorizonNode : public SLNode
{
public:
    SLHorizonNode(SLstring          name,
                  SLDeviceRotation* devRot,
                  SLTexFont*        font,
                  SLstring          shaderDir,
                  int               scrW,
                  int               scrH);
    ~SLHorizonNode();

    void doUpdate() override;

private:
    SLDeviceRotation* _devRot = nullptr;
    SLTexFont*        _font   = nullptr;
    SLstring          _shaderDir;

    SLGLProgram* _prog        = nullptr;
    SLMaterial*  _mat         = nullptr;
    SLPolyline*  _line        = nullptr;
    SLNode*      _horizonNode = nullptr;
    SLNode*      _textNode    = nullptr;

    SLMat3f _sRc;
};
//-----------------------------------------------------------------------------
#endif
