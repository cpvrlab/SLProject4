/**
 * \file      SLAssimpProgressHandler.h
 * \authors   Marcus Hudritsch
 * \date      December 2020
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLASSIMPPROGRESSHANDLER_H
#define SLASSIMPPROGRESSHANDLER_H

#ifdef SL_BUILD_WITH_ASSIMP
#    include <assimp/ProgressHandler.hpp>
#    include <AppCommon.h>

//-----------------------------------------------------------------------------
//!
class SLProgressHandler
{
public:
    virtual bool Update(float percentage = -1.f) = 0;
};
//-----------------------------------------------------------------------------
//!
class SLAssimpProgressHandler : SLProgressHandler
  , Assimp::ProgressHandler
{
public:
    virtual bool Update(float percentage = -1.f)
    {
        if (percentage >= 0.0f && percentage <= 100.0f)
        {
            AppCommon::jobProgressNum((SLint)percentage);
            return true;
        }
        else
            return false;
    }
};
//-----------------------------------------------------------------------------
#endif // SL_BUILD_WITH_ASSIMP
#endif // SLASSIMPPROGRESSHANDLER_H
