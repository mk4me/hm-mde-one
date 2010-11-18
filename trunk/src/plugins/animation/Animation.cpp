#include <plugins/animation/Animation.h>

#include <core/IServiceManager.h>
#include <plugins/animation/AnimationService.h>

using namespace std;

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

// #define SCALE 1
#define TIMERMULTIPLAY 0.009
#define ANIMATION_GROUP(pSkeletonNode) (*(pSkeletonNode)->GetAnimations())[_id]

//--------------------------------------------------------------------------------------------------
Animation::Animation(Skeleton* skeleton, SkeletonAnimation* skeletonAnimation, AnimationService* animationService):
m_pAnimationService(animationService)
, m_pSkeleton(skeleton)
, m_pSkeletonAnimaton(skeletonAnimation)
, _state(Animation::STOPPED)
, _actTime(0.0), _length(0.0)
, _bones(NULL)
, _id(0)
, _prevTime(0.0)
, _isStartAnimation(true)
, _firstNodeTime(0.0)
, SCALE(1.0)
{
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

        int animatioBoneCount = m_pSkeletonAnimaton->m_boneAnimationList.size();
		int boneCount = m_pSkeleton->m_pBoneList.size();

        for(int ab = 0; ab <animatioBoneCount; ab++)
        {
			for(int b = 0; b <boneCount; b++)
			{
				if(m_pSkeletonAnimaton->m_boneAnimationList[ab]->idx == m_pSkeleton->m_pBoneList[b]->idx)
				{
					   m_pSkeleton->m_pBoneList[b]->frame = m_pSkeletonAnimaton->m_boneAnimationList[ab]->m_frames;

//                     int childCount = m_pSkeleton->m_pBoneList[b]->child.size();
// 
//                     for(int c = 0 ; c < childCount; c++)
//                     {
//                         m_pSkeleton->m_pBoneList[b]->child[c]->frame = m_pSkeletonAnimaton->m_boneAnimationList[ab]->m_frames;
//                     }
				}
			}
        }

//         int childCount = m_pSkeleton->m_pRootBone->child.size();
// 
//         for(int c = 0 ; c < childCount; c++)
//         {
//             for(int ab = 0; ab <animatioBoneCount; ab++)
//             {
//                 if(m_pSkeleton->m_pRootBone->child[c]->idx == m_pSkeletonAnimaton->m_boneAnimationList[ab]->idx)
//                 {
//                     m_pSkeleton->m_pRootBone->child[c]->frame = m_pSkeletonAnimaton->m_boneAnimationList[ab]->m_frames;
//                 }
//             }
//         }
// 
// 
// 
// 
// 
//         m_pSkeleton->m_pRootBone->frame = m_pSkeletonAnimaton->m_boneAnimationList[0]->m_frames;

        // ilosc kosci = ilosc obiektów frame w dablicy
        m_pFrameCount = m_pSkeleton->m_pRootBone->frame.size();
        _length = m_pSkeleton->m_pRootBone->frame[m_pFrameCount-1]->m_time;
    }
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
void Animation::calculateChildMatrixBVHFormat( Bone* bone )
{
    //znalezienie odpowiedniej ramki uwzglêdniaj¹c czas
    int index = _actTime / TIMERMULTIPLAY;

    if(index > m_pFrameCount - 1)
    {
        _state = Animation::STOPPED;
        index = m_pFrameCount -1;
    }


    osg::Matrixd M;
    float rx, ry, rz;

    float x,y,z;
    float lenght = bone->length;
    x = bone->dir[0] * lenght;
    y = bone->dir[1] * lenght;
    z = bone->dir[2] * lenght;

    // trzeba uwzglêdniæ rotacje a asfa.

    // tworzenie macierzy animacji. AMC file
    // minusowe wartoœci dla uzyskania po³o¿enia wzglêdem œwiata + zamiana na radiany
    rx=-bone->frame[index]->rotx*M_PI/180.;
    ry=-bone->frame[index]->roty*M_PI/180.;
    rz=-bone->frame[index]->rotz*M_PI/180.;
    M.makeRotate(rx, osg::Vec3f(1.0f, 0.0f, 0.0f), ry, osg::Vec3f(0.0f, 0.0f, 1.0f), rz, osg::Vec3f(0.0f, 1.0f, 0.0f));
    M.setTrans(osg::Vec3f(x, z, y));

  
    *bone->matrix = M * (*bone->parent->matrix);
    //bone->matrix->postMultScale(osg::Vec3d(SCALE, SCALE, SCALE));

    osg::Vec3d trans = bone->matrix->getTrans();

    if(!strcmp(bone->name, "LeftFoot"))
        bool istrue = true;

    bone->positionx = trans.x() * SCALE;
    bone->positiony = trans.y() * SCALE;
    bone->positionz = trans.z() * SCALE;

    if(!bone->isInitialPosition)
    {
        bone->initialPosition = osg::Vec3d(bone->positionx, bone->positiony, bone->positionz);
        bone->isInitialPosition = true;
    }

    int childrenCount = bone->child.size();
    for(int i = 0; i<childrenCount; i++)
    {
        calculateChildMatrixBVHFormat(bone->child[i]);
    }
}

//--------------------------------------------------------------------------------------------------
void Animation::UpdateModelBVHFormat()
{
    // update skeleton
    // for every bone

    // OBLICZANIE ROOTA.

    //znalezienie odpowiedniej ramki uwzglêdniaj¹c czas
    int index = _actTime / TIMERMULTIPLAY;

    if(index > m_pFrameCount - 1)
    {
        _state = Animation::STOPPED;
        index = m_pFrameCount -1;
    }

    Bone* bone = m_pSkeleton->m_pRootBone;


    osg::Matrixd M;
    float rx, ry, rz;

    float x,y,z;
    float lenght = bone->length;
    x = (bone->dir[0] * lenght) + bone->frame[index]->translationx;
    y = (bone->dir[1] * lenght) + bone->frame[index]->translationy;
    z = (bone->dir[2] * lenght) + bone->frame[index]->translationz;



    // tworzenie macierzy animacji. AMC file
    // minusowe wartoœci dla uzyskania po³o¿enia wzglêdem œwiata + zamiana na radiany
    rx=-bone->frame[index]->rotx*M_PI/180.;
    ry=-bone->frame[index]->roty*M_PI/180.;
    rz=-bone->frame[index]->rotz*M_PI/180.;
    M.makeRotate(rx, osg::Vec3f(1.0f, 0.0f, 0.0f), ry, osg::Vec3f(0.0f, 0.0f, 1.0f), rz, osg::Vec3f(0.0f, 1.0f, 0.0f));
    M.setTrans(osg::Vec3f(x, z, y));


    *bone->matrix = M;

    osg::Vec3d trans = bone->matrix->getTrans();

    bone->positionx = trans.x() * SCALE;
    bone->positiony = trans.y() * SCALE;
    bone->positionz = trans.z() * SCALE;

    if(!bone->isInitialPosition)
    {
        bone->initialPosition = osg::Vec3d(bone->positionx, bone->positiony, bone->positionz);
        bone->isInitialPosition = true;
    }

    int childrenCount = bone->child.size();
    for(int i = 0; i<childrenCount; i++)
    {
        calculateChildMatrixBVHFormat(bone->child[i]);
    }
}

//--------------------------------------------------------------------------------------------------
void Animation::calculateChildMatrix(Bone *bone)
{
    //znalezienie odpowiedniej ramki uwzglêdniaj¹c czas
    int index = _actTime / TIMERMULTIPLAY;

    if(index > m_pFrameCount - 1)
    {
        _state = Animation::STOPPED;
        index = m_pFrameCount -1;
    }

    // C - macierz obrotów (axis) plik ASF
    // Cinv - inversja maierzy C
    // B - macierz pozycji bezwzglêdnej: offfset = direction*length - plik ASF
    // M - macierz ruchu - maciesz zaweiraj¹ca rotacj jak i translacje - plik AMC

    osg::Matrixd C, Cinv, B, M, tmp, tmp2;
    float rx, ry, rz;
    // minusowe wartoœci dla uzyskania po³o¿enia wzglêdem œwiata + zamiana na radiany
    rx=-bone->axis_x*M_PI/180.;
    ry=-bone->axis_y*M_PI/180.;
    rz=-bone->axis_z*M_PI/180.;
    // zmieniona rotacja ry = rz, rz = ry - prawdopodobnie osie w osg sa lewoskrêtne ??  czy¿by?
    C.makeRotate(rx, osg::Vec3f(1.0f, 0.0f, 0.0f), ry, osg::Vec3f(0.0f, 0.0f, 1.0f), rz, osg::Vec3f(0.0f, 1.0f, 0.0f));

    // tworzenie macierzy animacji. AMC file
    // minusowe wartoœci dla uzyskania po³o¿enia wzglêdem œwiata + zamiana na radiany
    rx=-bone->frame[index]->rotx*M_PI/180.;
    ry=-bone->frame[index]->roty*M_PI/180.;
    rz=-bone->frame[index]->rotz*M_PI/180.;
    // zmieniona rotacja ry = rz, rz = ry - prawdopodobnie osie w osg sa lewoskrêtne ??  czy¿by?
    M.makeRotate(rx, osg::Vec3f(1.0f, 0.0f, 0.0f), ry, osg::Vec3f(0.0f, 0.0f, 1.0f), rz, osg::Vec3f(0.0f, 1.0f, 0.0f));
    M.postMultTranslate(osg::Vec3f(bone->frame[index]->translationx * SCALE, bone->frame[index]->translationz * SCALE, bone->frame[index]->translationy * SCALE));

    float x,y,z;
    float lenght = bone->length;
    x = bone->dir[0] * lenght;
    y = bone->dir[1] * lenght;
    z = bone->dir[2] * lenght;


    B.makeTranslate(x, z, y);
    Cinv = osg::Matrixd::inverse(C);

    // L = CinvMCB - wzór na animacjie AMC file- jednakze osg ma macierz transponowane !!!!
    // konieczna jest zmian mnozenia, le¿ nie mo¿na zamienic stron mno¿enia z macierz¹ M (C i Cinv) !!!!
    tmp = M* C;
    tmp2 = Cinv * tmp;
    tmp = B * tmp2;
    *bone->matrix = tmp * (*bone->parent->matrix);
    //bone->matrix->postMultScale(osg::Vec3d(SCALE, SCALE, SCALE));

    osg::Vec3d trans = bone->matrix->getTrans();

    if(!strcmp(bone->name, "RightLeg"))
       bool istrue = true;

    bone->positionx = trans.x() * SCALE;
    bone->positiony = trans.y() * SCALE;
    bone->positionz = trans.z() * SCALE;

	if(!bone->isInitialPosition)
	{
		bone->initialPosition = osg::Vec3d(bone->positionx, bone->positiony, bone->positionz);
		bone->isInitialPosition = true;
	}

    int childrenCount = bone->child.size();
    for(int i = 0; i<childrenCount; i++)
    {
        calculateChildMatrix(bone->child[i]);
    }
}

//--------------------------------------------------------------------------------------------------
// update model - updates only mesh etc taking skeleton
// into consideration - called by update
void Animation::UpdateModel()
{
    // update skeleton
    // for every bone

    // OBLICZANIE ROOTA.

    //znalezienie odpowiedniej ramki uwzglêdniaj¹c czas
    int index = _actTime / TIMERMULTIPLAY;

    if(index > m_pFrameCount - 1)
    {
        _state = Animation::STOPPED;
        index = m_pFrameCount -1;
    }

    Bone* bone = m_pSkeleton->m_pRootBone;

    // C - macierz obrotów (axis) plik ASF
    // Cinv - inversja maierzy C
    // B - macierz pozycji bezwzglêdnej: offfset = direction*length - plik ASF
    // M - macierz ruchu - maciesz zaweiraj¹ca rotacj jak i translacje - plik AMC

    osg::Matrixd C, Cinv, B, M, tmp, tmp2;
    float rx, ry, rz;
    // minusowe wartoœci dla uzyskania po³o¿enia wzglêdem œwiata + zamiana na radiany
    rx=-bone->axis_x*M_PI/180.;
    ry=-bone->axis_y*M_PI/180.;
    rz=-bone->axis_z*M_PI/180.;

    // zmieniona rotacja ry = rz, rz = ry - prawdopodobnie osie w osg sa lewoskrêtne ??  czy¿by?
    C.makeRotate(rx, osg::Vec3f(1.0f, 0.0f, 0.0f), ry, osg::Vec3f(0.0f, 0.0f, 1.0f), rz, osg::Vec3f(0.0f, 1.0f, 0.0f));

    // tworzenie macierzy animacji. AMC file
    // minusowe wartoœci dla uzyskania po³o¿enia wzglêdem œwiata + zamiana na radiany
    rx=-bone->frame[index]->rotx*M_PI/180.;
    ry=-bone->frame[index]->roty*M_PI/180.;
    rz=-bone->frame[index]->rotz*M_PI/180.;
    M.makeRotate(rx, osg::Vec3f(1.0f, 0.0f, 0.0f), ry, osg::Vec3f(0.0f, 0.0f, 1.0f), rz, osg::Vec3f(0.0f, 1.0f, 0.0f));
    M.postMultTranslate(osg::Vec3f(bone->frame[index]->translationx, bone->frame[index]->translationz, bone->frame[index]->translationy));

   // M.makeRotate(0, osg::Vec3f(1.0f, 0.0f, 0.0f), 0, osg::Vec3f(0.0f, 0.0f, 1.0f), 0, osg::Vec3f(0.0f, 1.0f, 0.0f));
  //  M.postMultTranslate(osg::Vec3f(0, 0, 0));

    float x,y,z;
    float lenght = bone->length;
    x = bone->dir[0] * lenght;
    y = bone->dir[1] * lenght;
    z = bone->dir[2] * lenght;

    B.makeTranslate(x, z, y);
    Cinv = osg::Matrixd::inverse(C);

    // L = CinvMCB - wzór na animacjie AMC file- jednakze osg ma macierz transponowane !!!!
    // konieczna jest zmian mnozenia, le¿ nie mo¿na zamienic stron mno¿enia z macierz¹ M (C i Cinv) !!!!
    tmp = M * C;
    tmp2 = Cinv * tmp;
    *bone->matrix = B * tmp2;
    bone->matrix->postMultScale(osg::Vec3d(SCALE, SCALE, SCALE));

    osg::Vec3d trans = bone->matrix->getTrans();

    bone->positionx = trans.x() * SCALE;
    bone->positiony = trans.y() * SCALE;
    bone->positionz = trans.z() * SCALE;

	if(!bone->isInitialPosition)
	{
		bone->initialPosition = osg::Vec3d(bone->positionx, bone->positiony, bone->positionz);
		bone->isInitialPosition = true;
	}

    int childrenCount = bone->child.size();
    for(int i = 0; i<childrenCount; i++)
    {
        calculateChildMatrix(bone->child[i]);
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

//--------------------------------------------------------------------------------------------------
void Animation::SetScale( double scale )
{
	SCALE = scale;
}

//--------------------------------------------------------------------------------------------------
void Animation::calculateChildMatrix_ver2( Bone* bone )
{
    // update skeleton
    // for every bone

    // OBLICZANIE ROOTA.

    //znalezienie odpowiedniej ramki uwzglêdniaj¹c czas
    int index = _actTime / TIMERMULTIPLAY;

    if(index > m_pFrameCount - 1)
    {
        _state = Animation::STOPPED;
        index = m_pFrameCount -1;
    }

    osg::Matrixd temp;

    if(bone->frame.size() > index)
    {
        temp.setRotate(bone->frame[index]->rotation);
        temp.setTrans(bone->frame[index]->translation);
    }
    else
    {
        temp.setRotate(bone->rot);
        temp.setTrans(bone->trans);
    }

    *bone->matrix = temp;

    *bone->matrix =  (*bone->matrix) * (*bone->parent->matrix);

    osg::Vec3d trans = bone->matrix->getTrans();

    bone->positionx = trans.x();
    bone->positiony = trans.y();
    bone->positionz = trans.z();

    if(!bone->isInitialPosition)
    {
        bone->initialPosition = osg::Vec3d(bone->positionx, bone->positiony, bone->positionz);
        bone->isInitialPosition = true;
    }


    int childrenCount = bone->child.size();
    for(int i = 0; i<childrenCount; i++)
    {
        calculateChildMatrix_ver2(bone->child[i]);
    }
}

//--------------------------------------------------------------------------------------------------
void Animation::UpdateModel_ver2()
{
    // update skeleton
    // for every bone

    // OBLICZANIE ROOTA.

    //znalezienie odpowiedniej ramki uwzglêdniaj¹c czas
    int index = _actTime / TIMERMULTIPLAY;

    if(index > m_pFrameCount - 1)
    {
        _state = Animation::STOPPED;
        index = m_pFrameCount -1;
    }

    Bone* bone = m_pSkeleton->m_pRootBone;

    osg::Matrixd temp;

    temp.setRotate(bone->frame[index]->rotation);
    temp.setTrans(bone->frame[index]->translation);


    *bone->matrix = temp;

    osg::Vec3d trans = bone->matrix->getTrans();

    bone->positionx = trans.x();
    bone->positiony = trans.y();
    bone->positionz = trans.z();

    if(!bone->isInitialPosition)
    {
        bone->initialPosition = osg::Vec3d(bone->positionx, bone->positiony, bone->positionz);
        bone->isInitialPosition = true;
    }

    int childrenCount = bone->child.size();
    for(int i = 0; i<childrenCount; i++)
    {
        calculateChildMatrix_ver2(bone->child[i]);
    }
}

