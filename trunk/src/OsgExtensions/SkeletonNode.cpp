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
const std::vector<IAnimationGroup* >* SkeletonNode::GetAnimations()
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
void SkeletonNode::AddAnimation(IAnimationGroup* animation )
{
    _animations.push_back(animation); 
}

//--------------------------------------------------------------------------------------------------
int SkeletonNode::GetId()
{
    return _id; 
}

//--------------------------------------------------------------------------------------------------
void SkeletonNode::SetPosition( osg::Vec3d &pos )
{
    this->setPosition(pos);
}

//--------------------------------------------------------------------------------------------------
void SkeletonNode::SetAttitude( const osg::Quat& quat )
{
    this->setAttitude(quat);
}

//--------------------------------------------------------------------------------------------------
const osg::Vec3d& SkeletonNode::GetPosition() const
{
    return this->getPosition();
}

//--------------------------------------------------------------------------------------------------
const osg::Quat& SkeletonNode::GetAttitude() const
{
    return this->getAttitude();
}

//--------------------------------------------------------------------------------------------------
ISkeletonNode* SkeletonNode::GetParent(int i)
{
    return (ISkeletonNode*)this->getParent(i);
}