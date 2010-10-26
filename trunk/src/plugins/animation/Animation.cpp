#include <plugins/animation/Animation.h>

#include <core/IServiceManager.h>
#include <plugins/animation/AnimationService.h>

using namespace std;

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

#define TIMERMULTIPLAY 0.0055
#define ANIMATION_GROUP(pSkeletonNode) (*(pSkeletonNode)->GetAnimations())[_id]

//--------------------------------------------------------------------------------------------------
Animation::Animation(Skeleton* skeleton, AnimationService* animationService):
m_pAnimationService(animationService)
, m_pSkeleton(skeleton)
, _state(Animation::STOPPED)
, _actTime(0.0), _length(0.0)
, _bones(NULL)
, _id(0)
, _prevTime(0.0)
, _isStartAnimation(true)
, _firstNodeTime(0.0)
{
    //     // get length of this animation
    //     for (vector<ISkeletonNode*>::iterator i = _bones->begin(); i != _bones->end(); ++i)
    //         if((*i)->GetNumOfAnimations() > 0)
    //             if ((*i)->GetNumOfAnimations() && _length < ANIMATION_GROUP(*i)->getLength())
    //                 _length = ANIMATION_GROUP(*i)->getLength();

    // ilosc kosci = ilosc obiektów frame w dablicy
    m_pFrameCount = skeleton->m_pRootBone->frame.size();
    _length = skeleton->m_pRootBone->frame[m_pFrameCount-1]->m_time;
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
        m_pAnimationService->RegisterAnimation(this, &Animation::SetTime);
        m_pAnimationService->setLength(_length);

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

    // 	for (vector<ISkeletonNode*>::iterator i = _bones->begin(); i != _bones->end(); ++i)
    // 	{
    // 		// if animated bone
    // 		if ((*i)->GetNumOfAnimations())
    // 		{
    // 			// get animation group
    // 			IAnimationGroup* ag = ANIMATION_GROUP(*i);
    // 
    //             if(ag->getLength() <= 0)
    //                 continue;
    // 
    // 			// reset...
    // 			ag->setPosition(0);
    // 
    // 			// set key frame no. 0
    // 			osg::Vec3d zeroTrans = ag->getActNode()->GetPosition();
    // 			osg::Quat  zeroQuat  = ag->getActNode()->GetAttitude();
    // 
    // 			(*i)->SetPosition(zeroTrans);
    // 			(*i)->SetAttitude(zeroQuat);
    // 		}
    // 	}
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
        m_pAnimationService->RegisterAnimation(this, &Animation::SetTime);
        m_pAnimationService->setLength(_length);
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
void Animation::calculateMatrix(Bone *bone)
{
    int index = _actTime / TIMERMULTIPLAY;

    if(index > m_pFrameCount - 1)
    {
        _state = Animation::STOPPED;
        index = m_pFrameCount -1;
    }

    osg::Matrixd C1, Cinv1, B1, M1, tmp, tmp2;
    float rx, ry, rz;
    rx=-bone->axis_x*M_PI/180.;
    ry=-bone->axis_y*M_PI/180.;
    rz=-bone->axis_z*M_PI/180.;
    C1.makeRotate(rx, osg::Vec3f(1.0f, 0.0f, 0.0f), rz, osg::Vec3f(0.0f, 1.0f, 0.0f), ry, osg::Vec3f(0.0f, 0.0f, 1.0f));

    rx=-bone->frame[index]->rotx*M_PI/180.;
    ry=-bone->frame[index]->roty*M_PI/180.;
    rz=-bone->frame[index]->rotz*M_PI/180.;
    M1.makeRotate(rx, osg::Vec3f(1.0f, 0.0f, 0.0f), rz, osg::Vec3f(0.0f, 1.0f, 0.0f), ry, osg::Vec3f(0.0f, 0.0f, 1.0f));
    M1.postMultTranslate(osg::Vec3f(bone->frame[index]->translationx, bone->frame[index]->translationz, bone->frame[index]->translationy));

    float x,y,z;
    float lenght = bone->length;
    x = bone->dir[0] * lenght;
    y = bone->dir[1] * lenght;
    z = bone->dir[2] * lenght;


    B1.makeTranslate(x, z, y);
    Cinv1 = osg::Matrixd::inverse(C1);

    tmp = C1 * M1;
    tmp2 = tmp * Cinv1;
    tmp = B1 * tmp2;
    *bone->matrix = tmp * (*bone->parent->matrix);

    osg::Vec3d trans = bone->matrix->getTrans();

    bone->positionx = trans.x();
    bone->positiony = trans.y();
    bone->positionz = trans.z();


    int childrenCount = bone->child.size();
    for(int i = 0; i<childrenCount; i++)
    {
        calculateMatrix(bone->child[i]);
    }
}

//--------------------------------------------------------------------------------------------------
// update model - updates only mesh etc taking skeleton
// into consideration - called by update
void Animation::UpdateModel()
{
    // update skeleton
    // for every bone

    int index = _actTime / TIMERMULTIPLAY;

    if(index > m_pFrameCount - 1)
    {
        _state = Animation::STOPPED;
        index = m_pFrameCount -1;
    }

    int boundCount = m_pSkeleton->m_pBoneList.size();
    Bone* bone = m_pSkeleton->m_pRootBone;

    osg::Matrixd C1, Cinv1, B1, M1, tmp, tmp2;
    float rx, ry, rz;
    rx=-bone->axis_x*M_PI/180.;
    ry=-bone->axis_y*M_PI/180.;
    rz=-bone->axis_z*M_PI/180.;

    C1.makeRotate(rx, osg::Vec3f(1.0f, 0.0f, 0.0f), rz, osg::Vec3f(0.0f, 1.0f, 0.0f), ry, osg::Vec3f(0.0f, 0.0f, 1.0f));

    rx=-bone->frame[index]->rotx*M_PI/180.;
    ry=-bone->frame[index]->roty*M_PI/180.;
    rz=-bone->frame[index]->rotz*M_PI/180.;
    M1.makeRotate(rx, osg::Vec3f(1.0f, 0.0f, 0.0f), rz, osg::Vec3f(0.0f, 1.0f, 0.0f), ry, osg::Vec3f(0.0f, 0.0f, 1.0f));
    M1.postMultTranslate(osg::Vec3f(bone->frame[index]->translationx, bone->frame[index]->translationz, bone->frame[index]->translationy));

    float x,y,z;
    float lenght = bone->length;
    x = bone->dir[0] * lenght;
    y = bone->dir[1] * lenght;
    z = bone->dir[2] * lenght;

    B1.makeTranslate(x, z, y);
    Cinv1 = osg::Matrixd::inverse(C1);

    tmp = C1 * M1;
    tmp2 = tmp * Cinv1;
    *bone->matrix = tmp2 * B1;

    osg::Vec3d trans = bone->matrix->getTrans();

    bone->positionx = trans.x();
    bone->positiony = trans.y();
    bone->positionz = trans.z();


    int childrenCount = bone->child.size();
    for(int i = 0; i<childrenCount; i++)
    {
        calculateMatrix(bone->child[i]);
    }
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
    _actTime = dt;

    // update model
    UpdateModel();
}

//--------------------------------------------------------------------------------------------------
void Animation::SetTime( double time )
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
    //_actTime = time;

    // update model
    Update(time);
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

