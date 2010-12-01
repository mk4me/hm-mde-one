#include <plugins/animation/AnimationService.h>

#include <iostream>
#include <osgViewer/Scene>
#include <osg/Group>
#include <osg/Geode>
#include <osg/NodeVisitor>
#include <osg/Vec3d>
#include <osg/Quat>

#include <core/IC3DModel.h>
#include <core/IModel.h>
#include <core/IDataManager.h>
#include <core/IMesh.h>
#include <core/Vec3.h>
#include <core/ModelData.h>

#include <core/ISkeletonNode.h>
#include <core/IServiceManager.h>
#include <core/IRenderService.h>

#include <plugins/timeline/ITimeline.h>
#include <plugins/timeline/Stream.h>

#include "OsgControlWidget.h"

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
AnimationService::AnimationService(void): 
m_pAnimation(NULL)
, m_pC3DAnimation(NULL)
, m_pModel(NULL)
, m_pC3MModel(NULL)
, m_selectedAnimatonName("")
, targetTime(0.0)
, stateMutex()
, length(0.0)
, followTimeline(false)
, currentAnimation(NULL)
, c3dcurrentAnimation(NULL)
, name("Animation")
, SCALE(1)
{
    widget = new OsgControlWidget();
}

//--------------------------------------------------------------------------------------------------
AsyncResult AnimationService::init(IServiceManager* serviceManager, osg::Node* sceneRoot)
{
    m_pServiceManager = serviceManager;
    m_pScene = sceneRoot;
    if ( widget ) {
        widget->SetScene(sceneRoot, serviceManager);
    }

    m_pRenderService = dynamic_cast<IRenderService* >(m_pServiceManager->getService(UniqueID('REND','SRVC')).get());

    m_DisplayType = AnimasionDisplay::ALL;

    std::cout << "AnimationService ADDED!" << std::endl; 
    return AsyncResult_Complete; 
}

//--------------------------------------------------------------------------------------------------
AsyncResult AnimationService::loadData(IServiceManager* serviceManager, IDataManager* dataManager )
{
    Clear();

    m_pFactory = m_pRenderService->GetFactory();

    //nowy SetScene
	std::string meshpath = "";
	std::string skelpath = "";
	std::string c3dpath = "";
	if(dataManager->getMeshFilePathCount() > 0)
	{
		meshpath = dataManager->getMeshFilePath(0);
	}
	if(dataManager->getSkeletonFilePathCount() > 0)
	{
		skelpath = dataManager->getSkeletonFilePath(0);
	}
	if(dataManager->getC3dFilePathCount() > 0)
	{
		c3dpath = dataManager->getC3dFilePath(0);
	}

    std::vector<std::string> animationPathList = *dataManager->getAnimationPathList();

	m_pModel = m_pFactory->GetModel(meshpath, skelpath, animationPathList);// *dataManager->GetAnimationList());
	m_pC3MModel = m_pFactory->GetC3DModel(c3dpath);


    LoadAnimation(m_pModel);

	for (int i = 0; i < dataManager->getC3dFilePathCount(); i++)
        LoadAnimation(m_pFactory->GetC3DModel(dataManager->getC3dFilePath(i)));

    widget->SetScene(m_pScene, serviceManager);

    ITimelinePtr timeline = core::queryServices<ITimeline>(serviceManager);
    if ( timeline ) {
        timeline->addStream( timeline::StreamPtr(timeline::Stream::encapsulate(this)) );
    } else {
        OSG_WARN<<"ITimeline not found."<<std::endl;
    }

    return AsyncResult_Complete;
}

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
    RegisterFunction(object, fun);
	//m_pAnimation = new CSimpleOneArgFunctor<Animation, double>(object, fun);
}

void AnimationService::RegisterC3DAnimation(Animation* object, void (Animation::*fun)(double))
{
    // RegisterFunction(object, fun);
    m_pC3DAnimation = new CSimpleOneArgFunctor<Animation, double>(object, fun);
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
bool AnimationService::UnregisterC3DAnimation()
{
    // remove called functions etc if we have finished playing anim
    //if (getAnimation()->GetState() == EAnimationState::STOPPED)
    //	clearAll();

    // remove animation
    if (m_pC3DAnimation)
        delete m_pC3DAnimation;
    m_pC3DAnimation = NULL;

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
	SCALE = 1;
    m_numOfBones = 0;
    m_selectedAnimatonName= "";

   // widget = NULL;
    m_pModel = NULL;
    m_pJoints = NULL;
    m_skeleton = NULL;
    m_pC3MModel = NULL;
    m_pAnimation = NULL;
    m_pActualBones = NULL;
    m_pInitialBones = NULL;
    m_skeletonGeode = NULL;
  //  m_pServiceManager = NULL;

    m_animations.clear();
    m_c3danimations.clear();
    m_functionsToCall.clear();
    m_functionsToRemove.clear();
    m_functionsToCallWhenAnimationStopped.clear();

    m_animationNames.clear();
    m_c3dNames.clear();

    m_DisplayType = AnimasionDisplay::ALL;

	ClearCaller();
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
        targetTime = (std::min)(targetTime, length - 0.1);
    } else {
        if ( currentAnimation ) {
            targetTime = currentAnimation->GetTime() + timeDelta;
        } else {
            targetTime = 0.0;
        }
    }

    if(m_pModel)
    {
        if(m_DisplayType & AnimasionDisplay::MESH)
            UpdateMesh();

        if(m_DisplayType & AnimasionDisplay::BONE)
            m_pModel->DrawModelBone();
    }

    if(m_pC3MModel)
        if(m_DisplayType & AnimasionDisplay::MARKER)
            m_pC3MModel->DrawMarkers();

//     if(currentAnimation)
//         (*m_pAnimation)(targetTime);
// 
//     if(c3dcurrentAnimation)
//         (*m_pC3DAnimation)(targetTime);



    // call functions that are to call every animation frame
    for (std::vector<ISimpleOneArgFunctor<double>*>::iterator i = m_functionsToCall.begin(); i != m_functionsToCall.end(); ++i)
        (**i)(targetTime);

    // remove functions that are to remove...
    for (std::vector<std::vector<ISimpleOneArgFunctor<double>*>::iterator>::iterator i = m_functionsToRemove.begin(); 
        i != m_functionsToRemove.end(); ++i)
    {
        //delete (**i);
        m_functionsToCall.erase((const vector<ISimpleOneArgFunctor<double>*>::iterator)(*i));
        break;
    }


	m_functionsToRemove.clear();


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
    if(!model->GetSkeleton())
        return;

    m_pModel = model;
    unsigned int numOfAnims = 0;

    for(int i = 0; i < model->GetAnimation()->m_SkeletonAnimationList.size(); i++)
    {
        m_animationNames.push_back(model->GetAnimation()->m_SkeletonAnimationList[i]->m_animationName);
        Animation* animation = new Animation(model->GetSkeleton(), model->GetAnimation()->m_SkeletonAnimationList[i], this);
        m_animations.insert(make_pair(m_animationNames[i], animation));	
    }
}

//--------------------------------------------------------------------------------------------------
void AnimationService::LoadAnimation( IC3DModel* c3dModel )
{
    if(c3dModel->GetMarkerList().size() == 0)
        return;

    m_pC3MModel = c3dModel;

    // extract number of animations				
    unsigned int numOfAnims = 0;

    m_c3dNames.push_back(c3dModel->GetName());

    // create animations
    Animation* animation = new Animation(c3dModel->GetMarkerList(), this);
    m_c3danimations.insert(make_pair(c3dModel, animation));	
}

//--------------------------------------------------------------------------------------------------
void AnimationService::PlayAnimation(std::string animationName)
{
 //   map<std::string, Animation*>::iterator i = m_animations.find(animationName);
    
    map<std::string, Animation*>::iterator i;

    for(i = m_animations.begin(); i != m_animations.end(); i++)
    {
        if ( currentAnimation ) {
            currentAnimation->Stop();
        }
        
        if (i->first == animationName) 
        {
            currentAnimation = i->second;
            i->second->Play();
            break;
        } 
        else
        {
            ClearCaller();
            UnregisterAnimation();
            currentAnimation = NULL;
        }
    }

    // TODO: zrobiæ tak aby Geode by³o w jednym miejscu - jako singleton 
    // skruci temu podobne zamieszania.
    m_pRenderService->DisableBone();

    PlayC3DAnimation(animationName);
}

//--------------------------------------------------------------------------------------------------
void AnimationService::PlayC3DAnimation(std::string name)
{
    std::string c3dName;
    int nameCount = 0;
    bool find = false;
    for(int a = 0; a < m_animationNames.size(); a++)
    {
        if(m_animationNames[a] == name){
            find = true;
            break;
        }

        nameCount++;
    }

    c3dName = name.substr(0, name.find_last_of("."));
    c3dName.append(".c3d");

    if (c3dcurrentAnimation) {
        c3dcurrentAnimation->Stop();
    }

    if(m_c3danimations.size() < nameCount || !find)
        return;

    map<IC3DModel*, Animation*>::iterator i;
    for(i = m_c3danimations.begin(); i != m_c3danimations.end(); i++)
    {
        if (i->first->GetName() == c3dName) 
        {
            m_pC3MModel = i->first;
            m_pRenderService->SetC3DMarkerToRender(i->first);
            c3dcurrentAnimation = i->second;
            i->second->Play();
            break;
        } 
        else
        {
            m_pRenderService->DisableMarker();
            c3dcurrentAnimation = NULL;
        }
    }
}

//--------------------------------------------------------------------------------------------------
void AnimationService::UpdateMesh()
{
    if(!m_pModel->GetSkeleton())
        return;

    std::vector<IMesh*> meshList = m_pModel->GetMeshList();
    int countHandv = 0;

    std::vector<Bone*> m_pActualBones = m_pModel->GetSkeleton()->m_pBoneList;	

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

                    osg::Matrixd boneTransformation = m_pActualBones[boneID]->bonespace * (*m_pActualBones[boneID]->matrix);

                    // osg::Vec3f T = osg::Matrixd::transform3x3(*m_pActualBones[boneID]->matrix, m_pActualBones[boneID]->boneSpace_translation);
                    // T = m_pActualBones[boneID]->matrix->getTrans() + T;
                    // boneTransformation.setTrans(T);


                    osg::Vec3d temp = actPos * boneTransformation; 
                    _tempVectors[b][POSITION] = temp;

                    osg::Vec3d temp2 = boneTransformation.getRotate() * normal;
                    _tempVectors[b][NORMALS] = temp2;
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
IWidget* AnimationService::getWidget()
{
    return reinterpret_cast<IWidget*>(widget);
}

//--------------------------------------------------------------------------------------------------
double AnimationService::getTargetTime() const
{
    return targetTime;
}

//--------------------------------------------------------------------------------------------------
void AnimationService::setTargetTime( double time )
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(stateMutex);
    targetTime = time;
}

//--------------------------------------------------------------------------------------------------
double AnimationService::getLength() const
{
    return length;
}

//--------------------------------------------------------------------------------------------------
void AnimationService::setLength( double length )
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(stateMutex);
    this->length = length;
}

//--------------------------------------------------------------------------------------------------
bool AnimationService::getFollowTimeline() const
{
    return followTimeline;
}

//--------------------------------------------------------------------------------------------------
void AnimationService::setFollowTimeline( bool followTimeline )
{
    this->followTimeline = followTimeline;
}

//--------------------------------------------------------------------------------------------------
void AnimationService::setScale( float scale )
{
	SCALE = scale;
	if(currentAnimation)
		currentAnimation->SetScale(SCALE);
}

//--------------------------------------------------------------------------------------------------
// TODO: ujednoliœciæ kod  nie potrzebnie uzywamy 6 metod srobiæ jedna sprawdzajaca sendera
// Mo¿na to dowolnie zmieniæ.
void AnimationService::SetShowMesh( bool showMesh )
{
    if(showMesh)
    {
        m_pRenderService->EnableMesh();
        m_DisplayType = m_DisplayType | AnimasionDisplay::MESH;
    }
    else
    {
        m_pRenderService->DisableMesh();
        m_DisplayType = m_DisplayType ^ AnimasionDisplay::MESH;
    }

}

//--------------------------------------------------------------------------------------------------
void AnimationService::SetShowBone( bool showBone )
{
    if(showBone)
        m_DisplayType = m_DisplayType | AnimasionDisplay::BONE;
    else
    {
        m_pRenderService->DisableBone();
        m_DisplayType = m_DisplayType ^ AnimasionDisplay::BONE;
    }
}

//--------------------------------------------------------------------------------------------------
void AnimationService::SetShowMarker( bool showMarker )
{
    if(showMarker)
    {
        m_pRenderService->EnableMarker();
        m_DisplayType = m_DisplayType | AnimasionDisplay::MARKER;
    }
    else
    {
        m_pRenderService->DisableMarker();
        m_DisplayType = m_DisplayType ^ AnimasionDisplay::MARKER;
    }
}

