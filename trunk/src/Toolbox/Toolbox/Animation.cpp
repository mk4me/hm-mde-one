#include "Animation.h"


#include "ModelWithSkeleton.h"
#include "ServiceManager.h"
#include "AnimationService.h"


using namespace std;


#define ANIMATION_GROUP(pSkeletonNode) (*(pSkeletonNode)->GetAnimations())[_id].get()


//////////////////////////////////////////////////////////////////////////
// pause 
bool CAnimation::pause()
{
	if (_state == EAnimationState::PLAYING)
	{
		ServiceManager::GetInstance()->RegisterServiceAs<AnimationService>(); 
		AnimationService* animServ = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>(); 
		animServ->unregisterAnimation();

		_state = EAnimationState::PAUSED;

		return true;
	}

	return false;
}


//////////////////////////////////////////////////////////////////////////
// resume
bool CAnimation::resume()
{
	if (_state == EAnimationState::PAUSED)
	{
		ServiceManager::GetInstance()->RegisterServiceAs<AnimationService>(); 
		AnimationService* animServ = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>(); 
		animServ->registerAnimation(this, &CAnimation::update);

		_state = EAnimationState::PLAYING;

		return true;
	}

	return false;
}


//////////////////////////////////////////////////////////////////////////
// set transformations from frame 0
void CAnimation::firstFrame()
{
	_actTime	= 0.0;
	_prevTime	= 0.0;

	for (vector<CSkeletonNode*>::iterator i = _bones->begin(); i != _bones->end(); ++i)
	{
		// if animated bone
		if ((*i)->GetNumOfAnimations())
		{
			// get animation group
			CAnimationGroup* ag = ANIMATION_GROUP(*i);

            if(ag->getLength() <= 0)
                continue;

			// reset...
			ag->setPosition(0);

			// set key frame no. 0
			osg::Vec3d zeroTrans = ag->getActNode()->getPosition();
			osg::Quat  zeroQuat  = ag->getActNode()->getAttitude();

			(*i)->setPosition(zeroTrans);
			(*i)->setAttitude(zeroQuat);
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// stop
bool CAnimation::stop()
{
	firstFrame();

	// update model
	updateModel();
	_state = EAnimationState::STOPPED;

	ServiceManager::GetInstance()->RegisterServiceAs<AnimationService>(); 
	AnimationService* animServ = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>(); 
	animServ->notifyStop();

	return true;
}


//////////////////////////////////////////////////////////////////////////
// prepares animation for playing
void CAnimation::play(CModelWithSkeleton* model)
{ 
	// are we resuming?
	if (resume()) return;

	// add animation to caller
	if (_state != EAnimationState::PLAYING)
	{
		ServiceManager::GetInstance()->RegisterServiceAs<AnimationService>(); 
		AnimationService* animServ = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>(); 
		animServ->registerAnimation(this, &CAnimation::update);
		_state		= EAnimationState::PLAYING; 
	}

	_model		= model; 
//    _isStartAnimation = true;

//	firstFrame();
}


//////////////////////////////////////////////////////////////////////////
// moves to proper part of animation
double CAnimation::moveToProperPart(CAnimationGroup* ag)
{	
     if(_isStartAnimation)
     {
         // sprawdzenie pierwszego noda - i odjecie wartosci.  porzebne do przesuniecia czasu - poniewaz
         // nie kazda animacja zaczyna sie od czasu równego 0.
         _firstNodeTime = ag->getActNode()->GetTime();

         _length -= _firstNodeTime;
         _isStartAnimation = !_isStartAnimation;
     }

     //R.Z. trzeba odjac pasek przewijania od momentu startu.

	if	(_actTime > _prevTime)  // we are moving forwards
	{
		// actual node time
		double actNodeTime = ag->getActNode()->GetTime();
		if((_actTime + _firstNodeTime) > actNodeTime)
		{
			if (ag->next())
			{
				ag->goNext();
				return moveToProperPart(ag);
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
				    return moveToProperPart(ag);
			}
		}
	}

	// by default
	return 1.0;
}


//////////////////////////////////////////////////////////////////////////
// change animation progress
double CAnimation::setPogress(double t)
{
	EAnimationState::TYPE actState = _state;

	// put to <0, 1>
	t = (t < 0.0) ? 0.0 : 
		(t > 1.0) ? 1.0 : t;

	// set time
	_actTime = _length * t;

	// update Model
	updateModel();

	_state = actState;

	return _actTime;
}


//////////////////////////////////////////////////////////////////////////
// update model - updates only mesh etc taking skeleton
// into consideration - called by update
void CAnimation::updateModel()
{
	// update skeleton
	// for every bone
	for (vector<CSkeletonNode*>::iterator i =  _bones->begin(); i != _bones->end(); ++i)
	{
		// if animated bone
		if ((*i)->GetNumOfAnimations())
		{
			// get animation group
			CAnimationGroup* ag = ANIMATION_GROUP(*i);

			// get progress of actual part of animation

            if(ag->getLength() <=0)
                continue;

			double progress = moveToProperPart(ag);

			// transform bone
			osg::Vec3d oldTrans = (*i)->getPosition();
			osg::Quat  oldQuat  = (*i)->getAttitude();

			osg::Vec3d targetTrans = ag->getActNode()->getPosition();
			osg::Quat  targetQuat  = ag->getActNode()->getAttitude();

			// interpolate transformations
			osg::Vec3d newTrans = oldTrans + (targetTrans - oldTrans) * progress;
			osg::Quat  newQuat;  
			newQuat.slerp(progress, oldQuat, targetQuat);

			(*i)->setPosition(newTrans);
			(*i)->setAttitude(newQuat);

			// stop condition
			if (!ag->next() && fabs(progress - 1.0) < 0.000000001)
				_state = EAnimationState::STOPPED;
			else
				_state = EAnimationState::PLAYING;
		}
	}



	// update mesh
	_model->updateMesh();
	//	_model->drawNormals(2.0f);
	//	_model->test();
	//	_model->updateSkeletonMesh();

	// save previous time
	_prevTime = _actTime;
}


//////////////////////////////////////////////////////////////////////////
// update
void CAnimation::update(double dt)
{
	// if everything is done - stop animation
	if (_state == EAnimationState::STOPPED)
	{
		// remove animation from caller
		ServiceManager::GetInstance()->RegisterServiceAs<AnimationService>(); 
		AnimationService* animServ = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>(); 
		animServ->unregisterAnimation();

		firstFrame();

		return;
	}

	// update time flow
	_actTime += dt;

	// update model
	updateModel();
}


//////////////////////////////////////////////////////////////////////////
// c - tor
CAnimation::CAnimation(std::vector<CSkeletonNode*>* root, unsigned int id)
:	_state(EAnimationState::STOPPED), _actTime(0.0), _length(0.0), 
	_bones(root), _id(id), _model(NULL), _prevTime(0.0), _isStartAnimation(true), _firstNodeTime(0.0)
{
    // TODO: R.Z dlugos animacjia dokladnie ucieczka do wskaxnika na grupy animcji moze praowadzic do skoku w adres, oiekt 
    // który nie iestnieje - i czasm sie tak robi, przy ladowaniu wiêcej niz jednej animacji.

    // TODO: (kolejne spostrzezenie.  nie mamy tablicy animacji z SkeletonNode ANIMATION_GROUP czyli (pSkeletonNode) (*(pSkeletonNode)->GetAnimations())[_id].get()
    // zwróci nam przekroczenie zakresu wektora ( dla wiecej niz 1 animacja poniewaz size() _vector(...)_animation nie jest tablic¹ ( czy jak to uj¹æ :P to wskaŸnik - lista, nie musi miec tylu elementów ) 

	// get length of this animation
	for (vector<CSkeletonNode*>::iterator i = _bones->begin(); i != _bones->end(); ++i)
        if((*i)->GetNumOfAnimations() > 0)
	        if ((*i)->GetNumOfAnimations() && _length < ANIMATION_GROUP(*i)->getLength())
		        _length = ANIMATION_GROUP(*i)->getLength();
}