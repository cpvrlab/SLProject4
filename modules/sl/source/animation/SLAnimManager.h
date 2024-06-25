//#############################################################################
//  File:      SLAnimManager.h
//  Date:      Autumn 2014
//  Codestyle: https://code.google.com/p/slproject/wiki/CodingStyleGuidelines
//  Authors:   Marc Wacker, Marcus Hudritsch
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef SLANIMMANAGER_H
#define SLANIMMANAGER_H

#include <SLAnimPlayback.h>
#include <SLAnimation.h>
#include <SLAnimSkeleton.h>

//-----------------------------------------------------------------------------
//! SLAnimManager is the central class for all animation handling.
/*!
A single instance of this class is hold by the SLScene instance and is
responsible for updating the enabled animations and to manage their life time.
If keeps a list of all skeletons and node animations and also holds a list of
all animation playback controllers.
The update of all animations is done before the rendering of all SLSceneView in
SLScene::updateIfAllViewsGotPainted by calling the SLAnimManager::update.
*/
class SLAnimManager
{
public:
    ~SLAnimManager();

    void         addSkeleton(SLAnimSkeleton* skel);
    void         addNodeAnimation(SLAnimation* anim);
    SLAnimation* createNodeAnimation(SLfloat duration);
    SLAnimation* createNodeAnimation(const SLstring& name,
                                     SLfloat         duration);
    SLAnimation* createNodeAnimation(const SLstring& name,
                                     SLfloat         duration,
                                     SLbool          enabled,
                                     SLEasingCurve   easing,
                                     SLAnimLooping   looping);
    SLbool       hasNodeAnimations() { return (_animationNamesMap.size() > 0); }

    SLVSkeleton&     skeletons() { return _skeletons; }
    SLMAnimation&    animationNamesMap() { return _animationNamesMap; }
    SLMAnimPlayback& animPlaybackNamesMap() { return _animPlaybackNamesMap; }
    SLVstring&       animationNames() { return _animationNames; }
    SLVAnimPlayback& animPlaybacks() { return _animPlaybacks; }
    SLAnimPlayback*  animPlaybackByName(const SLstring& name);
    SLAnimPlayback*  animPlaybackByIndex(SLuint ix) { return _animPlaybacks[ix]; }
    SLAnimPlayback*  animPlaybacksBack() { return _animPlaybacks.back(); }

    SLbool update(SLfloat elapsedTimeSec);
    void   drawVisuals(SLSceneView* sv);
    void   clear();

private:
    SLVSkeleton     _skeletons;            //!< all skeleton instances
    SLMAnimation    _animationNamesMap;    //!< map name to animation
    SLMAnimPlayback _animPlaybackNamesMap; //!< map name to animation playbacks
    SLVstring       _animationNames;       //!< vector with all animation names
    SLVAnimPlayback _animPlaybacks;        //!< vector with all animation playbacks
};
//-----------------------------------------------------------------------------
#endif
