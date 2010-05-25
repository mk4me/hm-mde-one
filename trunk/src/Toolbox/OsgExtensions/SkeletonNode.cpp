#include "SkeletonNode.h"

using namespace osg;

//--------------------------------------------------------------------------------------------------
SkeletonNode::SkeletonNode():
  _id(-1)
{
}

//--------------------------------------------------------------------------------------------------
SkeletonNode::~SkeletonNode()
{
	_animations.clear();
}

//--------------------------------------------------------------------------------------------------
const std::vector<osg::ref_ptr<CAnimationGroup> >* SkeletonNode::GetAnimations()
{
    return &_animations; 
}

//--------------------------------------------------------------------------------------------------
unsigned int SkeletonNode::GetNumOfAnimations()
{
    return _animations.size(); 
}

//--------------------------------------------------------------------------------------------------
void SkeletonNode::SetId( int id )
{
    _id = id; 
}

//--------------------------------------------------------------------------------------------------
void SkeletonNode::AddAnimation( osg::ref_ptr<CAnimationGroup> animation )
{
    _animations.push_back(animation); 
}

//--------------------------------------------------------------------------------------------------
int SkeletonNode::GetId()
{
    return _id; 
}