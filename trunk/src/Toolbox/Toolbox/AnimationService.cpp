#include "AnimationService.h"


#include <iostream>
#include <osgViewer/Scene>
#include <osg/Group>
#include <osg/Geode>
#include <osg/NodeVisitor>

#include <SkeletonNode.h>


using namespace std;
using namespace osg;


M_DECLARED_CLASS(AnimationService, kCLASSID_AnimationService);


//--------------------------------------------------------------------------------------------------
// notify stop
void AnimationService::notifyStop()
{
	for (std::vector<ISimpleNoArgFunctor*>::iterator i = _functionsToCallWhenAnimationStopped.begin();
		i != _functionsToCallWhenAnimationStopped.end(); ++i)
	{
		(**i)();
	}
}


//--------------------------------------------------------------------------------------------------
// returns act animation
CAnimation* AnimationService::getAnimation()
{
	CSimpleOneArgFunctor<CAnimation, double>* anim = dynamic_cast<CSimpleOneArgFunctor<CAnimation, double>*>(_animation);

	if (anim)
		return anim->getObject();
	else
		return NULL;
}


//--------------------------------------------------------------------------------------------------
// clears caller
// add function to caller
void AnimationService::registerAnimation(CAnimation* object, void (CAnimation::*fun)(double))
{
	_animation = new CSimpleOneArgFunctor<CAnimation, double>(object, fun);
}


//--------------------------------------------------------------------------------------------------
// clears caller
// remove function from caller
bool AnimationService::unregisterAnimation()
{
	// remove called functions etc if we have finished playing anim
	//if (getAnimation()->GetState() == EAnimationState::STOPPED)
	//	clearAll();

	// remove animation
	if (_animation)
		delete _animation;
	_animation = NULL;

	return true;
}


//--------------------------------------------------------------------------------------------------
// clears caller
void AnimationService::clearCaller()
{
	for (std::vector<ISimpleOneArgFunctor<double>*>::iterator i = _functionsToCall.begin(); i != _functionsToCall.end(); ++i)
		delete (*i);
	for (std::vector<ISimpleNoArgFunctor*>::iterator i = _functionsToCallWhenAnimationStopped.begin();
		i != _functionsToCallWhenAnimationStopped.end(); ++i)
		delete (*i);
	
	_functionsToCallWhenAnimationStopped.clear();
	_functionsToCall.clear();
	_functionsToRemove.clear();
}


//--------------------------------------------------------------------------------------------------
void AnimationService::clearAll()
{
	// caller
	clearCaller();
}


//--------------------------------------------------------------------------------------------------
AnimationService::AnimationService(void)
: _animation(NULL), _selectedAnimatonName("")
{
}


//--------------------------------------------------------------------------------------------------
AnimationService::~AnimationService(void)
{
	clearAll();
}


//--------------------------------------------------------------------------------------------------
AsyncResult AnimationService::OnTick(double delta)
{ 
	if (_animation)
	{
		// update animation
		(*_animation)(delta);

		// call functions that are to call every animation frame
		for (std::vector<ISimpleOneArgFunctor<double>*>::iterator i = _functionsToCall.begin(); i != _functionsToCall.end(); ++i)
			(**i)(delta);

		// remove functions that are to remove...
		for (std::vector<std::vector<ISimpleOneArgFunctor<double>*>::iterator>::iterator i = _functionsToRemove.begin(); 
			i != _functionsToRemove.end(); ++i)
		{
			delete (**i);
			_functionsToCall.erase((const vector<ISimpleOneArgFunctor<double>*>::iterator)(*i));
		}
		_functionsToRemove.clear();
	}

    return AsyncResult_Complete; 
}


//--------------------------------------------------------------------------------------------------
AsyncResult AnimationService::OnAdded()
{
    std::cout << "AnimationService ADDED!" << std::endl; 
    return AsyncResult_Complete; 
}


//--------------------------------------------------------------------------------------------------
void AnimationService::SetScene(osgViewer::Scene* scene)
{
    std::cout << "AnimationService: Scene added" << std::endl; 
    _scene = scene; 
}
