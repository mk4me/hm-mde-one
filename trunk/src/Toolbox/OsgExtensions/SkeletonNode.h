#ifndef SKELETON_NODE_H
#define SKELETON_NODE_H


#include <osg/Group>
#include <osg/PositionAttitudeTransform>
#include "AnimationGroup.h"


//////////////////////////////////////////////////////////////////////////
// adds to PositionAttitudeTransform field id
class CSkeletonNode : public osg::PositionAttitudeTransform
{
		// id
		int											_id;

		// animation
		std::vector<osg::ref_ptr<CAnimationGroup> >	_animations;

	public:

		// add animation
		void AddAnimation(osg::ref_ptr<CAnimationGroup> animation)			{ _animations.push_back(animation); }
		
		// set id
		void setId(int id)													{ _id = id; }

		// get id
		int  getId()														{ return _id; }
	
		// number of animations
		unsigned int GetNumOfAnimations()									{ return _animations.size(); }

		// get animation
		const std::vector<osg::ref_ptr<CAnimationGroup> >* GetAnimations()	{ return &_animations; }

		// c - tor
		CSkeletonNode();

	protected:

		// d - tor
		virtual ~CSkeletonNode();
};

#endif