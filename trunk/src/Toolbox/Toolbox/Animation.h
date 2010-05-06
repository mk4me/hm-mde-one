#ifndef ANIMATION_H
#define ANIMATION_H


#include <SkeletonNode.h>
#include <AnimationNode.h>
#include <AnimationGroup.h>

// #include "ModelWithSkeleton.h"
class CModelWithSkeleton;

//////////////////////////////////////////////////////////////////////////
// states of animations
namespace EAnimationState
{
	enum TYPE
	{
		STOPPED = 0,	// animation is stopped
		PAUSED,			// pause
		PLAYING			// we are plying it right now
	};
};
// TODO:
// zastanowic sie czy nie zrobic tego troche inaczej...

//////////////////////////////////////////////////////////////////////////
// representation of animation
class CAnimation
{
		// act time of animation
		double							_actTime;

		// last time of animation (are we going backwards of forwards?)
		double							_prevTime;

		// length of animation
		double							_length;

		// state of animation
		EAnimationState::TYPE			_state;

		// animated bones
		std::vector<CSkeletonNode*>*	_bones;

		// id of animation
		unsigned int					_id;

		// pointer to object being animated
		CModelWithSkeleton*				_model;

		// moves to proper part of animation and returns %
		// of actual part progress
		double moveToProperPart(CAnimationGroup* ag);

		// update model - updates only mesh etc taking skeleton
		// into consideration - called by update
		void updateModel();

		// set transformations from frame 0
		void firstFrame();

        bool                            _isStartAnimation;

        double                          _firstNodeTime;

	public:

		// play
		void play(CModelWithSkeleton* model);

		// pause 
		bool pause();

		// stop
		bool stop();

		// resume
		bool resume();

		// update
		void update(double dt);

		// sets model
		void setModel(CModelWithSkeleton* model) { _model = model; } 

		// change animation progress
		// value <0.0, 1.0>
		double setPogress(double t);

		// progress of animation
		double getProgress()	{ return _actTime / _length; }

		// act time of animation
		double getTime()		{ return _actTime; }

		// get state of this animation
		EAnimationState::TYPE	GetState()		{ return _state; }

		// c - tor
		CAnimation(std::vector<CSkeletonNode*>* root, unsigned int id);
};

#endif