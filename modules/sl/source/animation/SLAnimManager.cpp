//#############################################################################
//  File:      SLAnimManager.cpp
//  Date:      Autumn 2014
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marc Wacker, Marcus Hudritsch
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! destructor
SLAnimManager::~SLAnimManager()
{
    clear();
}

//-----------------------------------------------------------------------------
//! Clears and deletes all node animations and skeletons
void SLAnimManager::clear()
{
    for (const auto& it : _animationNamesMap)
        delete it.second;
    _animationNamesMap.clear();

    for (const auto& it : _animPlaybackNamesMap)
        delete it.second;
    _animPlaybackNamesMap.clear();

    for (auto* skeleton : _skeletons)
        delete skeleton;
    _skeletons.clear();

    _animationNames.clear();
    _animPlaybacks.clear();
}
//-----------------------------------------------------------------------------
//! Add a skeleton to the skeleton vector
void SLAnimManager::addSkeleton(SLAnimSkeleton* skel)
{
    _skeletons.push_back(skel);
}
//-----------------------------------------------------------------------------
/*! Creates a new node animation
    @param  duration    length of the animation
*/
SLAnimation* SLAnimManager::createNodeAnimation(SLfloat duration)
{
    SLuint             index = (SLuint)_animationNamesMap.size();
    std::ostringstream oss;

    do
    {
        oss.clear();
        oss << "Node_" << index;
        index++;
    } while (_animationNamesMap.find(oss.str()) != _animationNamesMap.end());

    return createNodeAnimation(oss.str(), duration);
}
//-----------------------------------------------------------------------------
/*! Creates new SLAnimation istance for node animations. It will already create
 and set parameters for the respective SLAnimPlayback.
*/
SLAnimation* SLAnimManager::createNodeAnimation(const SLstring& name,
                                                SLfloat         duration,
                                                SLbool          enabled,
                                                SLEasingCurve   easing,
                                                SLAnimLooping   looping)
{
    SLAnimation*    anim     = createNodeAnimation(name, duration);
    SLAnimPlayback* playback = animPlaybackByName(name);
    playback->enabled(enabled);
    playback->easing(easing);
    playback->loop(looping);
    return anim;
}
//-----------------------------------------------------------------------------
/*! Creates a new node animation
    @param  name        the animation name
    @param  duration    length of the animation
*/
SLAnimation* SLAnimManager::createNodeAnimation(const SLstring& name,
                                                SLfloat         duration)
{
    assert(_animationNamesMap.find(name) == _animationNamesMap.end() &&
           "node animation with same name already exists!");

    SLAnimation* anim        = new SLAnimation(name, duration);
    _animationNamesMap[name] = anim;

    SLAnimPlayback* playback    = new SLAnimPlayback(anim);
    _animPlaybackNamesMap[name] = playback;

    // Add node animation to the combined vector
    _animationNames.push_back(name);
    _animPlaybacks.push_back(playback);

    return anim;
}
//-----------------------------------------------------------------------------
//! Returns the playback of a node animation or skeleton by name if it exists.
SLAnimPlayback* SLAnimManager::animPlaybackByName(const SLstring& name)
{
    for(auto playback : _animPlaybacks)
    {
        if (playback->parentAnimation()->name() == name)
            return playback;
    }


    SL_WARN_MSG("*** SLAnimManager::animPlaybackByName: animation not found ***");
    return nullptr;
}
//-----------------------------------------------------------------------------
//! Advances the time of all enabled animation plays.
SLbool SLAnimManager::update(SLfloat elapsedTimeSec)
{
    // reset the dirty flag on all skeletons
    for (auto* skeleton : _skeletons)
        skeleton->changed(false);

    SLbool updated = false;

    // advance time for node animations and apply them
    // @todo currently we can't blend between normal node animations because we
    // reset them per animation playback. so the last playback that affects a
    // node will have its animation applied.
    // We need to save the playback differently if we want to blend them.

    for (const auto& it : _animPlaybackNamesMap)
    {
        SLAnimPlayback* playback = it.second;
        if (playback->enabled())
        {
            playback->parentAnimation()->resetNodes();
            playback->advanceTime(elapsedTimeSec);
            playback->parentAnimation()->apply(playback->localTime(),
                                               playback->weight());
            updated = true;
        }
    }

    // update the skeletons separately
    for (auto* skeleton : _skeletons)
        updated |= skeleton->updateAnimations(elapsedTimeSec);

    return updated;
}
//-----------------------------------------------------------------------------
//! Draws the animation visualizations.
void SLAnimManager::drawVisuals(SLSceneView* sv)
{
    for (const auto& it : _animPlaybackNamesMap)
    {
        SLAnimPlayback* playback = it.second;
        playback->parentAnimation()->drawNodeVisuals(sv);
    }

    // skeletons are drawn from within SLSceneView per node
}
//-----------------------------------------------------------------------------
