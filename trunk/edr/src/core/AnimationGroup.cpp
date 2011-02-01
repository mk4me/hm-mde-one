#include "CorePCH.h"
#include "AnimationGroup.h"


//--------------------------------------------------------------------------------------------------
// get actual animation node
IAnimationNode*	CAnimationGroup::getNextNode()
{ 
	return dynamic_cast<IAnimationNode*>(getChild(_counter + 1)); 
}


//--------------------------------------------------------------------------------------------------
// get previous animation node
IAnimationNode*	CAnimationGroup::getPrevNode()		
{ 
	return dynamic_cast<IAnimationNode*>(getChild(_counter - 1)); 
}


//--------------------------------------------------------------------------------------------------
CAnimationGroup::~CAnimationGroup()
{
}


//--------------------------------------------------------------------------------------------------
CAnimationGroup::CAnimationGroup(unsigned int n)
: _length(0.0f), _frames(n), _counter(0)
{

}

//--------------------------------------------------------------------------------------------------
std::string CAnimationGroup::GetName()
{
	return this->getName();
}