/**
 * \file      CVRaulMurExtNode.h
 * \brief     Declares the Raul Mur ORB feature detector and descriptor
 * \details   This File is based on the ORB Implementation of ORB_SLAM
 *            https://github.com/raulmur/ORB_SLAM2
 * \date      Spring 2017
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 * \authors   Pascal Zingg, Timon Tschanz, Michael Goettlicher, Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
*/

#ifndef CVRAULMUREXTNODE_H
#define CVRAULMUREXTNODE_H

#include <CVTypedefs.h>

using std::list;

//-----------------------------------------------------------------------------
//! Data structure used to subdivide the Image with key points into segments.
class CVRaulMurExtNode
{
public:
    CVRaulMurExtNode() : bNoMore(false) {}

    void DivideNode(CVRaulMurExtNode& n1,
                    CVRaulMurExtNode& n2,
                    CVRaulMurExtNode& n3,
                    CVRaulMurExtNode& n4);

    CVVKeyPoint                      vKeys;
    CVPoint2i                        UL, UR, BL, BR;
    list<CVRaulMurExtNode>::iterator lit;
    bool                             bNoMore;
};
//-----------------------------------------------------------------------------
#endif // CVRAULMUREXTRACTORNODE_H
