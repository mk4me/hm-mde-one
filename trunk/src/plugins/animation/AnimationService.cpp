#include <plugins/animation/AnimationService.h>

#include <iostream>
#include <osgViewer/Scene>
#include <osg/Group>
#include <osg/Geode>
#include <osg/NodeVisitor>
#include <osg/Vec3d>
#include <osg/Quat>

#include <core/IModel.h>
#include <core/IDataManager.h>
#include <core/IMesh.h>
#include <core/Vec3.h>
#include <core/ModelData.h>

#include <core/ISkeletonNode.h>
#include <core/IServiceManager.h>

#include <plugins/timeline/ITimeline.h>
#include <plugins/timeline/Stream.h>

#include "QtWidget.h"

using namespace std;
using namespace osg;

//deprecated:
//M_DECLARED_CLASS(AnimationService, kCLASSID_AnimationService);

// TODO przenieœæ to do osobnego pliku!
class AnimationStream : public timeline::Stream
{
private:
  //! Strumieñ wewnêtrzny.
  AnimationService* service;

public:
  //! \param stream Strumieñ wewnêtrzny.
  AnimationStream(AnimationService* service) : service(service)
  {}
  //! \see Stream::setTime
  virtual void setTime(double time)
  {
    service->setTargetTime(time);
  }
  //! \see Stream::getTime
  virtual double getTime() const
  {
    return service->getTargetTime();
  }
  //! \see Stream::getLength
  virtual double getLength() const
  {
    return service->getLength();
  }
};

////////////////////////////////////////////////////////////////////////////////
namespace timeline {
////////////////////////////////////////////////////////////////////////////////
template<> inline Stream* Stream::encapsulate(AnimationService* service)
{
  return new AnimationStream(service);
}
////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////


#define pPat osg::PositionAttitudeTransform*
#define POSITION 0
#define NORMALS  1

//--------------------------------------------------------------------------------------------------
// notify stop
void AnimationService::NotifyStop()
{
	for (std::vector<ISimpleNoArgFunctor*>::iterator i = m_functionsToCallWhenAnimationStopped.begin();
		i != m_functionsToCallWhenAnimationStopped.end(); ++i)
	{
		(**i)();
	}
}

//--------------------------------------------------------------------------------------------------
// returns act animation
Animation* AnimationService::GetAnimation()
{
	CSimpleOneArgFunctor<Animation, double>* anim = dynamic_cast<CSimpleOneArgFunctor<Animation, double>*>(m_pAnimation);

	if (anim)
		return anim->getObject();
	else
		return NULL;
}

//--------------------------------------------------------------------------------------------------
// clears caller
// add function to caller
void AnimationService::RegisterAnimation(Animation* object, void (Animation::*fun)(double))
{
	m_pAnimation = new CSimpleOneArgFunctor<Animation, double>(object, fun);
}

//--------------------------------------------------------------------------------------------------
// clears caller - remove function from caller
bool AnimationService::UnregisterAnimation()
{
	// remove called functions etc if we have finished playing anim
	//if (getAnimation()->GetState() == EAnimationState::STOPPED)
	//	clearAll();

	// remove animation
	if (m_pAnimation)
		delete m_pAnimation;
	m_pAnimation = NULL;

	return true;
}

//--------------------------------------------------------------------------------------------------
// clears caller
void AnimationService::ClearCaller()
{
	for (std::vector<ISimpleOneArgFunctor<double>*>::iterator i = m_functionsToCall.begin(); i != m_functionsToCall.end(); ++i)
		delete (*i);
	for (std::vector<ISimpleNoArgFunctor*>::iterator i = m_functionsToCallWhenAnimationStopped.begin();
		i != m_functionsToCallWhenAnimationStopped.end(); ++i)
		delete (*i);
	
	m_functionsToCallWhenAnimationStopped.clear();
	m_functionsToCall.clear();
	m_functionsToRemove.clear();
}

//--------------------------------------------------------------------------------------------------
void AnimationService::Clear()
{
    m_numOfBones = 0;
    m_selectedAnimatonName= "";

    m_pModel = NULL;
    m_pJoints = NULL;
    m_skeleton = NULL;
    m_pAnimation = NULL;
    m_pActualBones = NULL;
    m_skeletonGeode = NULL;
    m_pInitialBones = NULL;

    m_animations.clear();
    m_functionsToCall.clear();
    m_functionsToRemove.clear();
    m_functionsToCallWhenAnimationStopped.clear();

	ClearCaller();

    
}

//--------------------------------------------------------------------------------------------------
AnimationService::AnimationService(void): 
  m_pAnimation(NULL)
, m_selectedAnimatonName(""),
  targetTime(0.0),
  stateMutex(),
  length(0.0),
  followTimeline(false),
  currentAnimation(NULL),
  name("Animation")
{
    widget = new QtWidget;
}

//--------------------------------------------------------------------------------------------------
AnimationService::~AnimationService(void)
{
	Clear();
}

//--------------------------------------------------------------------------------------------------
AsyncResult AnimationService::update(double time, double timeDelta)
{ 
    double targetTime;
    if ( followTimeline ) {
        targetTime = this->targetTime;
        // HACK: ¿eby animacja siê nie "blokowa³a" (Piotr Gwiazdowski)
        targetTime = std::min(targetTime, length - 0.1);
    } else {
        if ( currentAnimation ) {
            targetTime = currentAnimation->GetTime() + timeDelta;
        } else {
            targetTime = 0.0;
        }
    }

	if (m_pAnimation)
	{
		// update animation
		(*m_pAnimation)(targetTime);
  //      UpdateSkeleton();
  //      RecalculateChanges();
  //      UpdateMesh();

		m_pModel->DrawModelBone();

		// call functions that are to call every animation frame
		for (std::vector<ISimpleOneArgFunctor<double>*>::iterator i = m_functionsToCall.begin(); i != m_functionsToCall.end(); ++i)
			(**i)(targetTime);

		// remove functions that are to remove...
		for (std::vector<std::vector<ISimpleOneArgFunctor<double>*>::iterator>::iterator i = m_functionsToRemove.begin(); 
			i != m_functionsToRemove.end(); ++i)
		{
			delete (**i);
			m_functionsToCall.erase((const vector<ISimpleOneArgFunctor<double>*>::iterator)(*i));
		}
		m_functionsToRemove.clear();
	}

    return AsyncResult_Complete; 
}

//--------------------------------------------------------------------------------------------------
AsyncResult AnimationService::init(IServiceManager* serviceManager, osg::Node* sceneRoot)
{
    m_pServiceManager = serviceManager;
    m_pScene = sceneRoot;
    if ( widget ) {
        widget->SetData(sceneRoot, serviceManager);
    }

    std::cout << "AnimationService ADDED!" << std::endl; 
    return AsyncResult_Complete; 
}

//--------------------------------------------------------------------------------------------------
void AnimationService::SetSelectedAnimationName(const std::string& name)
{ 
    m_selectedAnimatonName = name; 
} 

//--------------------------------------------------------------------------------------------------
// returns name of act selected animation
std::string& AnimationService::GetSelectedAnimationName() 
{ 
    return m_selectedAnimatonName; 
}

//--------------------------------------------------------------------------------------------------
void AnimationService::LoadAnimation( IModel* model )
{
    Clear();

    m_pModel = model;

//     m_pJoints = model->GetJoints();
//     m_skeleton = (osg::Group*)model->GetSkeletonGroup();
// 
//     if(!m_pJoints || !m_skeleton)
//         return;
// 
// 
//     m_numOfBones = m_pJoints->size();
//     m_numOfBones += 1; // because we don't added first node (root_of_Skeleton)
// 
//     // get 'global' (? - not another transformations but entire propagated from root) 
//     // transformations of bones
//     m_pActualBones = new STransform [m_numOfBones];
//     UpdateSkeleton();
// 
//     // save initial values
//     m_pInitialBones = new STransform [m_numOfBones];
//     memcpy(m_pInitialBones, m_pActualBones, sizeof(STransform) * m_numOfBones);

    //////////////////////////////////////////////////////////////////////////
    // handle animations

    // extract number of animations				
    unsigned int numOfAnims = 0;
    vector<string> names;
//     for (vector<ISkeletonNode*>::iterator i = m_pJoints->begin(); i != m_pJoints->end(); ++i)
//     {
//         if ((*i)->GetNumOfAnimations() > numOfAnims)
//         {					
//             for (unsigned int j = numOfAnims; j < (*i)->GetNumOfAnimations(); ++j)
//                 names.push_back((*(*i)->GetAnimations())[j]->GetName());
// 
//             numOfAnims = (*i)->GetNumOfAnimations();
//         }
//     }

	names.push_back("anim_test");
	numOfAnims++;
//    int counting = m_pJoints->size();

    // create animations
    for (unsigned int i = 0; i < numOfAnims; ++i)
    {
        Animation* animation = new Animation(model->GetSkeleton(), this);
        m_animations.insert(make_pair(names[i], animation));	
    }
    
}


//--------------------------------------------------------------------------------------------------
// update skeleton
void AnimationService::UpdateSkeleton()
{
    pPat skeleton = dynamic_cast<pPat>(m_skeleton.get());
    if (skeleton)
    {
        for (unsigned int b = 0; b < skeleton->getNumChildren(); ++b)
            if (dynamic_cast<ISkeletonNode*>(skeleton->getChild(b)) )
                UpdateBone((pPat)skeleton->getChild(b));
    }
}

//--------------------------------------------------------------------------------------------------
// creates arrays of pointers at bones sorted by id
void AnimationService::UpdateBone(osg::PositionAttitudeTransform* bone)
{
    unsigned int pId = (dynamic_cast<ISkeletonNode*>(bone->getParent(0)))->GetId();
    unsigned int boneId = (dynamic_cast<ISkeletonNode*>(bone))->GetId();

    std::string name = bone->getName();


    osg::Vec3d ppos = pId ? m_pActualBones[pId].position : ((pPat)m_skeleton.get())->getPosition();
    osg::Quat  prot = pId ? m_pActualBones[pId].attitude : ((pPat)m_skeleton.get())->getAttitude();

    osg::Vec3d bpos = prot * bone->getPosition() + ppos;
    osg::Quat  brot = bone->getAttitude() * prot;

    STransform tr;
    tr.attitude	= brot;
    tr.position	= bpos;

    tr.bone		= bpos - ppos;
    tr.point	= ppos;

    if (bone->getNumChildren() > 0)
        tr.hasChild = true;
    else
        tr.hasChild = false;

    m_pActualBones[boneId] = tr;

    for (unsigned int b = 0; b < bone->getNumChildren(); ++b)
        if (dynamic_cast<ISkeletonNode*>(bone->getChild(b)))
            UpdateBone((pPat)bone->getChild(b));

}

//--------------------------------------------------------------------------------------------------
void AnimationService::PlayAnimation(std::string animationName)
{
    map<std::string, Animation*>::iterator i = m_animations.find(animationName);

    if ( currentAnimation ) {
        currentAnimation->Stop();
    }
    
    if (i != m_animations.end()) {
        currentAnimation = i->second;
        i->second->Play();
    } else {
        UnregisterAnimation();
        currentAnimation = NULL;
    }
}

//--------------------------------------------------------------------------------------------------
// recalculates changes of skeleton
void AnimationService::RecalculateChanges()
{
    for (unsigned int b = 0; b < (m_numOfBones + 1); ++b)
    {
        // cross product
        Vec3d	cross	 = m_pInitialBones[b].bone ^ m_pActualBones[b].bone;	
        // cos of angle between old and new bone
        double  cosangle = (m_pInitialBones[b].bone * m_pActualBones[b].bone) / (m_pInitialBones[b].bone.length() * m_pActualBones[b].bone.length());
        // angle between old and new bone
        double	angle    = acos(cosangle);	// acos of dot product div by lenths

        // construct bone rotation...
        m_pActualBones[b].rotation	= Quat(angle, cross);
        // ...transformation
        m_pActualBones[b].translation = m_pActualBones[b].point - m_pInitialBones[b].point;
    }
}
//--------------------------------------------------------------------------------------------------
void AnimationService::UpdateMesh()
{
    std::vector<IMesh*> meshList = m_pModel->GetMeshList();
    int countHandv = 0;

    for(std::vector<IMesh*>::iterator it = meshList.begin(); it != meshList.end(); it++)
    {
        IMesh* mesh = *it;
        SSkin* skin = NULL;

        if (mesh && (skin = mesh->GetSkin()))
        {
            // update vertices using skin
            for (int i = 0; i < skin->n; i++)
            {
                // vertice
                SSkinnedVertice* vertice = &skin->skinned_vertices[i];

                // act position of vertice
                Vec3d actPos(mesh->GetRootVerts()[vertice->vert_id]._v[0],
                    mesh->GetRootVerts()[vertice->vert_id]._v[1], mesh->GetRootVerts()[vertice->vert_id]._v[2]);

                // normal
                Vec3d normal(mesh->GetRootVertNormals()[vertice->vert_id]._v[0], 
                    mesh->GetRootVertNormals()[vertice->vert_id]._v[1], mesh->GetRootVertNormals()[vertice->vert_id]._v[2]);


                // for every affecting bone
                for (int b = 0; b < vertice->n; b++)
                {
                    int boneID = vertice->bones[b].boneID;
                    if(m_pActualBones[boneID].hasChild)
                    {
                        boneID++;  // rotacje bierzemy z dziecka. Z konca koœci która jest pocz¹tkiem nastêpnej.
                    }

                    _tempVectors[b][POSITION] =	m_pActualBones[boneID].rotation 
                        * (actPos - m_pActualBones[boneID].point									
                        + m_pActualBones[boneID].translation)									
                        + m_pActualBones[boneID].point;

                    _tempVectors[b][NORMALS]  = m_pActualBones[vertice->bones[b].boneID].rotation * normal;

                }

                // get output point
                Vec3d change, nchange;
                for (int b = 0; b < vertice->n; b++)
                {
                    change  += _tempVectors[b][POSITION] * vertice->bones[b].weight;
                    nchange += _tempVectors[b][NORMALS]  * vertice->bones[b].weight;
                }

                // update of vertex
                mesh->GetVerts()[vertice->vert_id]._v[0] = change.x();
                mesh->GetVerts()[vertice->vert_id]._v[1] = change.y();
                mesh->GetVerts()[vertice->vert_id]._v[2] = change.z();

                // update of normal 
                mesh->GetVertNormals()[vertice->vert_id]._v[0] = nchange.x();
                mesh->GetVertNormals()[vertice->vert_id]._v[1] = nchange.y();
                mesh->GetVertNormals()[vertice->vert_id]._v[2] = nchange.z();
            }
        }

        (*it)->Update();
    }
}

//--------------------------------------------------------------------------------------------------
std::map<std::string, Animation*>* AnimationService::GetAnimations()
{
    return &m_animations;
}

//--------------------------------------------------------------------------------------------------
AsyncResult AnimationService::loadData(IServiceManager* serviceManager, IDataManager* dataManager )
{
    LoadAnimation(dataManager->GetModel());
    widget->SetData(m_pScene, serviceManager);

    ITimelinePtr timeline = serviceManager->queryServices<ITimeline>();
    if ( timeline ) {
        timeline->addStream( timeline::StreamPtr(timeline::Stream::encapsulate(this)) );
    } else {
        OSG_WARN<<"ITimeline not found."<<std::endl;
    }

    return AsyncResult_Complete;
}

IWidget* AnimationService::getWidget()
{
    return reinterpret_cast<IWidget*>(widget->GetWidget());
}

QtWidget* AnimationService::getQtWidget()
{
    return widget;
}

double AnimationService::getTargetTime() const
{
    return targetTime;
}

void AnimationService::setTargetTime( double time )
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(stateMutex);
    targetTime = time;
}

double AnimationService::getLength() const
{
    return length;
}

void AnimationService::setLength( double length )
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(stateMutex);
    this->length = length;
}

bool AnimationService::getFollowTimeline() const
{
    return followTimeline;
}

void AnimationService::setFollowTimeline( bool followTimeline )
{
    this->followTimeline = followTimeline;
}