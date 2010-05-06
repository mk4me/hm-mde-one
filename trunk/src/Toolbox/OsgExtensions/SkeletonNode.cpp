#include "SkeletonNode.h"

using namespace osg;

CSkeletonNode::CSkeletonNode()
: _id(-1)
{
}


CSkeletonNode::~CSkeletonNode()
{
	_animations.clear();
}
