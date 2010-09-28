#include "CorePCH.h"
#include "AnimationNode.h"

//--------------------------------------------------------------------------------------------------
CAnimationNode::CAnimationNode(double time)
: _time(time)
{
}

//--------------------------------------------------------------------------------------------------
const osg::Vec3d& CAnimationNode::GetPosition() const
{
    return this->getPosition();
}

//--------------------------------------------------------------------------------------------------
const osg::Quat& CAnimationNode::GetAttitude() const
{
    return this->getAttitude();
}

//--------------------------------------------------------------------------------------------------
void CAnimationNode::SetPosition( osg::Vec3d& pos )
{
    this->setPosition(pos);
}

//--------------------------------------------------------------------------------------------------
void CAnimationNode::SetAttitude( osg::Quat& quat )
{
    this->setAttitude(quat);
}