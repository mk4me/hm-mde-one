// HACK: dziêki temu, ¿e src/core/Frame.h i include/core/Frame.h
// maj¹ tego samego ifndefa, mo¿na przy odpowiedniej kolejnoœci
// wybraæ ten plik, o który nam chodzi. Takie rozwi¹zanie jest jednak
// hackiem i trzeba to po prostu uporz¹dkowaæ
#include "../src/core/Frame.h"
#include <plugins/animation/Animation.h>


#include <core/IServiceManager.h>
#include <plugins/animation/AnimationService.h>
#include <core/Transform.h>
#include <core/Bone.h>

using namespace std;

#define TIMERMULTIPLAY 0.02
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

	double C[4][4];
	double Cinv[4][4];
	double B[4][4];

	double M[4][4];



	int i;
	double Rx[4][4], Ry[4][4], Rz[4][4], tmp[4][4], tmp2[4][4];

	//motion matrix
	rotationZ(Rz, bone->frame[index]->rotz);
	rotationY(Ry, bone->frame[index]->roty);
	rotationX(Rx, bone->frame[index]->rotx);
	matrix_mult(Rz, Ry, tmp);
	matrix_mult(tmp, Rx, M);

	M[0][3]=bone->frame[index]->translationx;
	M[1][3]=bone->frame[index]->translationy;
	M[2][3]=bone->frame[index]->translationz;



	rotationZ(Rz, bone->axis_z);
	rotationY(Ry, bone->axis_y);
	rotationX(Rx, bone->axis_x);
	matrix_mult(Rz, Ry, tmp);
	matrix_mult(tmp, Rx, C);

	//matrix_transpose(tmp2, C);   

	float x,y,z;
	float lenght = bone->length;
	x = bone->dir[0] * lenght;
	y = bone->dir[1] * lenght;
	z = bone->dir[2] * lenght;

	LoadFromTranslationVec(x,y,z,B);
	//matrix_transpose(tmp, B);   

	//CopyMatrix(Cinv,C);

	for ( int i = 0; i < 4; i++ )
		for ( int j = 0; j < 4; j++ ) 
		{
			Cinv[i][j] = C[i][j];
		}

		// 		osg::Matrixf mtrix = osg::Matrixf::inverse(osg::Matrixf(C[0][0], C[0][1], C[0][2], C[0][3],
		// 																C[1][0], C[1][1], C[1][2], C[1][3],
		// 																C[2][0], C[2][1], C[2][2], C[2][3],
		// 																C[3][0], C[3][1], C[3][2], C[3][3]));

		Invert(Cinv);

		SetToProduct(Cinv, M, tmp);
		SetToProduct(tmp, C, tmp2);
		SetToProduct(tmp2, B, tmp);
		SetToProduct(bone->parent->matrix, tmp, bone->matrix);

		bone->positionx = bone->matrix[0][3];
		bone->positiony = bone->matrix[1][3];
		bone->positionz = bone->matrix[2][3];



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

	double C[4][4];
	double Cinv[4][4];
	double B[4][4];

	double M[4][4];

	

	int i;
	double Rx[4][4], Ry[4][4], Rz[4][4], tmp[4][4], tmp2[4][4];

	//motion matrix
	rotationZ(Rz, bone->frame[index]->rotz);
	rotationY(Ry, bone->frame[index]->roty);
	rotationX(Rx, bone->frame[index]->rotx);
	matrix_mult(Rz, Ry, tmp);
	matrix_mult(tmp, Rx, M);

	M[0][3]=bone->frame[index]->translationx;
	M[1][3]=bone->frame[index]->translationy;
	M[2][3]=bone->frame[index]->translationz;



	rotationZ(Rz, bone->axis_z);
	rotationY(Ry, bone->axis_y);
	rotationX(Rx, bone->axis_x);
	matrix_mult(Rz, Ry, tmp);
	matrix_mult(tmp, Rx, C);

	//matrix_transpose(tmp2, C);   

	float x,y,z;
	float lenght = bone->length;
	x = bone->dir[0] * lenght;
	y = bone->dir[1] * lenght;
	z = bone->dir[2] * lenght;

	LoadFromTranslationVec(x,y,z,B);
	//matrix_transpose(tmp, B);   

	//CopyMatrix(Cinv,C);

	for ( int i = 0; i < 4; i++ )
		for ( int j = 0; j < 4; j++ ) 
		{
			Cinv[i][j] = C[i][j];
		}

		// 		osg::Matrixf mtrix = osg::Matrixf::inverse(osg::Matrixf(C[0][0], C[0][1], C[0][2], C[0][3],
		// 																C[1][0], C[1][1], C[1][2], C[1][3],
		// 																C[2][0], C[2][1], C[2][2], C[2][3],
		// 																C[3][0], C[3][1], C[3][2], C[3][3]));

		Invert(Cinv);

		SetToProduct(Cinv, M, tmp);
		SetToProduct(tmp, C, tmp2);
		SetToProduct(tmp2, B, bone->matrix);

		bone->positionx = bone->matrix[0][3];
		bone->positiony = bone->matrix[1][3];
		bone->positionz = bone->matrix[2][3];



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

