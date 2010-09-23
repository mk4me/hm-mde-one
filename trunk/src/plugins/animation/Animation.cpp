#include "Animation.h"

#include <core/IServiceManager.h>
#include "AnimationService.h"

using namespace std;

#define ANIMATION_GROUP(pSkeletonNode) (*(pSkeletonNode)->GetAnimations())[_id]

//--------------------------------------------------------------------------------------------------
Animation::Animation(std::vector<ISkeletonNode*>* root, unsigned int id, AnimationService* animationService):
  m_pAnimationService(animationService)
, _state(Animation::STOPPED)
, _actTime(0.0), _length(0.0)
, _bones(root)
, _id(id)
, _prevTime(0.0)
, _isStartAnimation(true)
, _firstNodeTime(0.0)
{
    // get length of this animation
    for (vector<ISkeletonNode*>::iterator i = _bones->begin(); i != _bones->end(); ++i)
        if((*i)->GetNumOfAnimations() > 0)
            if ((*i)->GetNumOfAnimations() && _length < ANIMATION_GROUP(*i)->getLength())
                _length = ANIMATION_GROUP(*i)->getLength();
}

//--------------------------------------------------------------------------------------------------
bool Animation::Pause()
{
	if (_state == Animation::PLAYING)
	{
		m_pAnimationService->UnregisterAnimation();

		_state = Animation::PAUSED;

		return true;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------
bool Animation::Resume()
{
	if (_state == Animation::PAUSED)
	{
		m_pAnimationService->RegisterAnimation(this, &Animation::Update);

		_state = Animation::PLAYING;

		return true;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------
// Set transformations from frame 0
void Animation::FirstFrame()
{
	_actTime	= 0.0;
	_prevTime	= 0.0;

	for (vector<ISkeletonNode*>::iterator i = _bones->begin(); i != _bones->end(); ++i)
	{
		// if animated bone
		if ((*i)->GetNumOfAnimations())
		{
			// get animation group
			IAnimationGroup* ag = ANIMATION_GROUP(*i);

            if(ag->getLength() <= 0)
                continue;

			// reset...
			ag->setPosition(0);

			// set key frame no. 0
			osg::Vec3d zeroTrans = ag->getActNode()->GetPosition();
			osg::Quat  zeroQuat  = ag->getActNode()->GetAttitude();

			(*i)->SetPosition(zeroTrans);
			(*i)->SetAttitude(zeroQuat);
		}
	}
}

//--------------------------------------------------------------------------------------------------
bool Animation::Stop()
{
	FirstFrame();

	// update model
	UpdateModel();
	_state = Animation::STOPPED;

	m_pAnimationService->NotifyStop();

	return true;
}

//--------------------------------------------------------------------------------------------------
// Prepares animation for playing
void Animation::Play()
{ 
	// are we resuming?
	if (Resume()) return;

	// add animation to caller
	if (_state != Animation::PLAYING)
	{
		m_pAnimationService->RegisterAnimation(this, &Animation::Update);
		_state = Animation::PLAYING; 
	}

}

//--------------------------------------------------------------------------------------------------
// moves to proper part of animation
double Animation::MoveToProperPart(IAnimationGroup* ag)
{	
    if(_isStartAnimation)
    {
        // sprawdzenie pierwszego noda - i odjecie wartosci.  porzebne do przesuniecia czasu - poniewaz
        // nie kazda animacja zaczyna sie od czasu równego 0.
        _firstNodeTime = ag->getActNode()->GetTime();

        _length -= _firstNodeTime;
        _isStartAnimation = !_isStartAnimation;
    }

    //FIXME: ??? trzeba odjac pasek przewijania od momentu startu.
    if	(_actTime > _prevTime)  // we are moving forwards
    {
        // actual node time
        double actNodeTime = ag->getActNode()->GetTime();
        if((_actTime + _firstNodeTime) > actNodeTime)
        {
            if (ag->next())
            {
                ag->goNext();
                return MoveToProperPart(ag);
            }
            // else : 1.0
        }
        else if	((_actTime + _firstNodeTime) < actNodeTime)
        {
            if (ag->prev())
            {
                double prevNodeTime = ag->getPrevNode()->GetTime();
                double progress = ((_actTime + _firstNodeTime) - prevNodeTime) / (actNodeTime - prevNodeTime);
                // return progress of transformation
                return progress;
            }
            return (_actTime + _firstNodeTime) / actNodeTime;
        }
    }
    else if	(_actTime < _prevTime)	// we are moving backwards
    {
        // actual node time
        double actNodeTime = ag->getActNode()->GetTime();
        if((_actTime + _firstNodeTime) > actNodeTime)
        {
            if (ag->next())
            {
                double nextNodeTime = ag->getNextNode()->GetTime();
                // return progress of transformation
                return (nextNodeTime - (_actTime + _firstNodeTime)) / (nextNodeTime - actNodeTime);
            }
        }
        else if ((_actTime + _firstNodeTime) < actNodeTime)
        {
            if (ag->prev())
            {
                ag->goPrev();
                if(ag->prev())
                    return MoveToProperPart(ag);
            }
        }
    }

    // by default
    return 1.0;
}

//--------------------------------------------------------------------------------------------------
// change animation progress
double Animation::SetPogress(double t)
{
	Animation::AnimationState actState = _state;
	// put to <0, 1>
	t = (t < 0.0) ? 0.0 : (t > 1.0) ? 1.0 : t;
	_actTime = _length * t;
	// update Model
	UpdateModel();
	_state = actState;
	return _actTime;
}

//--------------------------------------------------------------------------------------------------
// update model - updates only mesh etc taking skeleton
// into consideration - called by update
void Animation::UpdateModel()
{
	// update skeleton
	// for every bone
	for (vector<ISkeletonNode*>::iterator i = _bones->begin(); i != _bones->end(); ++i)
	{
		// if animated bone
		if ((*i)->GetNumOfAnimations())
		{
			// get animation group
			IAnimationGroup* ag = ANIMATION_GROUP(*i);

			// get progress of actual part of animation

            if(ag->getLength() <=0)
                continue;

			double progress = MoveToProperPart(ag);

			// transform bone
			osg::Vec3d oldTrans = (*i)->GetPosition();
			osg::Quat  oldQuat  = (*i)->GetAttitude();

			osg::Vec3d targetTrans = ag->getActNode()->GetPosition();
			osg::Quat  targetQuat  = ag->getActNode()->GetAttitude();

			// interpolate transformations
			osg::Vec3d newTrans = oldTrans + (targetTrans - oldTrans) * progress;
			osg::Quat  newQuat;  
			newQuat.slerp(progress, oldQuat, targetQuat);

			(*i)->SetPosition(newTrans);
			(*i)->SetAttitude(newQuat);

			// stop condition
			if (!ag->next() && fabs(progress - 1.0) < 0.000000001)
				_state = Animation::STOPPED;
			else
				_state = Animation::PLAYING;
		}
	}



	// update mesh
	
    //_model->Update();
	
    //	_model->drawNormals(2.0f);
	//	_model->test();
	//	_model->updateSkeletonMesh();

	// save previous time
	_prevTime = _actTime;
}

//--------------------------------------------------------------------------------------------------
void Animation::Update(double dt)
{
	// if everything is done - stop animation
	if (_state == Animation::STOPPED)
	{
		// remove animation from caller
		m_pAnimationService->UnregisterAnimation();

		FirstFrame();

		return;
	}

	// update time flow
	_actTime += dt;

	// update model
	UpdateModel();
}

//--------------------------------------------------------------------------------------------------
double Animation::GetProgress()
{
    return _actTime / _length; 
}

//--------------------------------------------------------------------------------------------------
double Animation::GetTime()
{
    return _actTime; 
}

//--------------------------------------------------------------------------------------------------
Animation::AnimationState Animation::GetState()
{
    return _state; 
}