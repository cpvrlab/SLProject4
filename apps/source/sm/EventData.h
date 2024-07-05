/**
 * \file      EventData.h
 * \brief     Event class used in the state machine 
 * \authors   Michael Göttlicher
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 */
#ifndef SM_EVENT_DATA_H
#define SM_EVENT_DATA_H

namespace sm
{

//-----------------------------------------------------------------------------
class EventData
{
public:
    virtual ~EventData() {}
};
//-----------------------------------------------------------------------------
class NoEventData : public EventData
{
public:
    NoEventData()
    {
    }
};
//-----------------------------------------------------------------------------
}
#endif
