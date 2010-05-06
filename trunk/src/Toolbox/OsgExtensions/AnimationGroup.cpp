#include "AnimationGroup.h"


//////////////////////////////////////////////////////////////////////////
// get actual animation node
CAnimationNode*	CAnimationGroup::getNextNode()
{ 
	return dynamic_cast<CAnimationNode*>(getChild(_counter + 1)); 
}


//////////////////////////////////////////////////////////////////////////
// get previous animation node
CAnimationNode*	CAnimationGroup::getPrevNode()		
{ 
	return dynamic_cast<CAnimationNode*>(getChild(_counter - 1)); 
}


//////////////////////////////////////////////////////////////////////////
// d - tor
CAnimationGroup::~CAnimationGroup()
{
}


//////////////////////////////////////////////////////////////////////////
// c - tor
CAnimationGroup::CAnimationGroup(unsigned int n)
: _length(0.0f), _frames(n), _counter(0)
{

}