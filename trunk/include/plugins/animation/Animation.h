#ifndef ANIMATION_H
#define ANIMATION_H


#include <core/ISkeletonNode.h>
#include <core/IAnimationNode.h>
#include <core/IAnimationGroup.h>
#include <core/Skeleton.h>
#include <core/Frame.h>


class IModel;
class AnimationService;

// TODO:
// zastanowic sie czy nie zrobic tego troche inaczej...

//--------------------------------------------------------------------------------------------------
// Representation of animation
class Animation
{
public:
    // States of the animation 
    enum AnimationState
    {
        STOPPED = 0,	// animation is stopped
        PAUSED,			// pause
        PLAYING			// we are plying it right now
    };

public: 
    Animation(Skeleton* skeleton, AnimationService* animationService);

    // play
    void Play();
    // pause 
    bool Pause();
    // stop
    bool Stop();
    // resume
    bool Resume();
    // update
    void Update(double dt);
    void SetTime(double time);

    // change animation progress
    // value <0.0, 1.0>
    double SetPogress(double t);
    // progress of animation
    double GetProgress(); 
    // act time of animation
    double GetTime(); 
    // get state of this animation
    Animation::AnimationState GetState(); 
    
private: 
    // moves to proper part of animation and returns %
    // of actual part progress
    double MoveToProperPart(IAnimationGroup* ag);
    // update model - updates only mesh etc taking skeleton
    // into consideration - called by update
    void UpdateModel();
    // set transformations from frame 0
    void FirstFrame();

	void calculateMatrix(Bone* bone);

private: 

	Skeleton* m_pSkeleton;
	int m_pFrameCount;

    // act time of animation
    double _actTime;
    // last time of animation (are we going backwards of forwards?)
    double _prevTime;
    // length of animation
    double _length;
    // state of animation
    Animation::AnimationState _state;
    // animated bones
    std::vector<ISkeletonNode*>* _bones;
    // id of animation
    unsigned int _id;
    // pointer to object being animated

    bool _isStartAnimation;
    double _firstNodeTime;

    AnimationService* m_pAnimationService;
};

#endif
