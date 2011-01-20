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

#pragma warning (disable: 4018)

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
, m_pC3DModel(NULL)
, m_selectedAnimatonName("")
, targetTime(0.0)
, stateMutex()
, length(0.0)
, followTimeline(true)
, currentAnimation(NULL)
, c3dcurrentAnimation(NULL)
, name("Animation")
, SCALE(1)
{
    widget = new OsgControlWidget();
}

//--------------------------------------------------------------------------------------------------
AsyncResult AnimationService::init(IServiceManager* serviceManager, IDataManager* dataManager, osg::Node* sceneRoot, osgViewer::CompositeViewer* viewer)
{
    m_pServiceManager = serviceManager;
    m_pScene = sceneRoot;
    if ( widget ) {
        widget->SetScene(sceneRoot, serviceManager);
    }

    m_pRenderService = dynamic_cast<IRenderService* >(m_pServiceManager->getService(UniqueID('REND','SRVC')).get());
    m_DisplayType = ALL;
    return AsyncResult_Complete; 
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
    m_pC3DModel = NULL;
    m_pAnimation = NULL;
    m_pActualBones = NULL;
    m_pInitialBones = NULL;
    m_skeletonGeode = NULL;
    //  m_pServiceManager = NULL;

    m_BoneUniform = NULL;
    m_BoneSpaceUniform = NULL;

    m_animations.clear();
    m_c3danimations.clear();
    m_functionsToCall.clear();
    m_functionsToRemove.clear();
    m_animationDisplayList.clear();
    m_functionsToCallWhenAnimationStopped.clear();

    m_DisplayType = ALL;

    ClearCaller();
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
	if(dataManager->getActualLocalTrial().isSkeleton())
	{
		skelpath = dataManager->getActualLocalTrial().getSkeletonPath();
	}
	if(dataManager->getActualLocalTrial().isC3d())
	{
		c3dpath = dataManager->getActualLocalTrial().getC3dPath();
	}
	//if(dataManager->getSkeletonFilePathCount() > 0)
	//{
	//	skelpath = dataManager->getSkeletonFilePath(0);
	//}
	//if(dataManager->getC3dFilePathCount() > 0)
	//{
	//	c3dpath = dataManager->getC3dFilePath(0);
	//}

    std::vector<std::string> animationPathList = dataManager->getActualLocalTrial().getAnimationsPaths();//*dataManager->getAnimationPathList();

    // uzyskanie obiekty klasy model i C3DModel poprzez fabryke modeli
	m_pModel = m_pFactory->GetModel(meshpath, skelpath, animationPathList);
	m_pC3DModel = m_pFactory->GetC3DModel(c3dpath);

    // ladowanie modelu animacji animacji - poprzez dane zapisane w obiekcie kalsu Model
    LoadAnimation(m_pModel);

    // za³adownie wszystkich modeli typu 3cd - poniewaz modele c3d same w sobie sa odrebnym modelem posiadajacym w³asna animacje
	//for (int i = 0; i < dataManager->getC3dFilePathCount(); i++)
 //       LoadAnimation(m_pFactory->GetC3DModel(dataManager->getC3dFilePath(i)));
	if(dataManager->getActualLocalTrial().isC3d())
	{
		LoadAnimation(m_pFactory->GetC3DModel(dataManager->getActualLocalTrial().getC3dPath()));
	}

    widget->SetScene(m_pScene, serviceManager);

    timeline = core::queryServices<ITimeline>(serviceManager);
    if ( timeline ) {
        timeline->addStream( timeline::StreamPtr(timeline::Stream::encapsulate(this)) );
    } else {
        OSG_WARN<<"ITimeline not found."<<std::endl;
    }

    // szukanie sciezki odpowiedniego shadera - przy ewentualnym wowo³anie Hardware  animation
    for(int i = 0; i < dataManager->getShaderFilePathCount(); i++)
    {
        std::string shaderName = dataManager->getShaderFilePath(i);
        if(shaderName.substr(shaderName.find_last_of("/") + 1, shaderName.length()) == "skinning.vert")
        {
            m_HardwareAnimationShaderPath = dataManager->getShaderFilePath(i);
            break;
        }

    }

    InicializeAnimation();

    return AsyncResult_Complete;
}

//--------------------------------------------------------------------------------------------------
void AnimationService::InicializeAnimation()
{
    if(!m_animations.empty()){
        m_animations.begin()->second->PlayFirstFrame();
        PlayAnimation(m_animations.begin()->first);
    }

    InitMarkerList();
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
        }else if(!currentAnimation && c3dcurrentAnimation){
            targetTime = c3dcurrentAnimation->GetTime() + timeDelta;
        }
        else {
            targetTime = 0.0;
        }
    }

    if(m_pModel)
    {
        if(m_DisplayType & MESH)
            GPUUpdateMesh();

        if(m_DisplayType & BONE)
            m_pModel->DrawModelBone();
    }

    if(m_pC3DModel)
        if(m_DisplayType & MARKER)
            m_pC3DModel->DrawMarkers();


    // call functions that are to call every animation frame
    for (std::vector<ISimpleOneArgFunctor<double>*>::iterator i = m_functionsToCall.begin(); i != m_functionsToCall.end(); ++i)
        (**i)(targetTime);

    // Ma³y Trick R.Z - metoda end() pojeminika nigdy nie wskazuje na ostati element - tylko na ten znajdujacy sie za nim. a on zawsze bedzie ró¿ny niz ten od m_functionsToCall
    // Prawid³owo nie uzywa sie tych meoda w ten sposób. TODO: w przysz³oœci w razie problemów zast¹piæ czymœ innym
    // remove functions that are to remove...
    for (std::vector<std::vector<ISimpleOneArgFunctor<double>*>::iterator>::iterator i = m_functionsToRemove.begin(); i != m_functionsToRemove.end();)
    {
        if(m_functionsToCall.empty())
            break;


        delete (**i);

        //*i = m_functionsToCall.erase((const vector<ISimpleOneArgFunctor<double>*>::iterator)(*i));
        m_functionsToCall.erase((const vector<ISimpleOneArgFunctor<double>*>::iterator)(*i));
        i = m_functionsToRemove.erase(i);
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
    if(!model || !model->GetSkeleton())
        return;

    m_pModel = model;
    unsigned int numOfAnims = 0;

    // utworzenie tylu obiektów Animation ile animacji posaida Model / usupe³nienie mapy animacji, jak i listy wyswietlajacej pasiadane animacje
    for(int i = 0; i < model->GetAnimationList()->m_SkeletonAnimationList.size(); i++)
    {
        Animation* animation = new Animation(model->GetSkeleton(), model->GetAnimationList()->m_SkeletonAnimationList[i], this);
        m_animations.insert(make_pair(model->GetAnimationList()->m_SkeletonAnimationList[i]->m_animationName, animation));	
        m_animationDisplayList.push_back(model->GetAnimationList()->m_SkeletonAnimationList[i]->m_animationName);
    }
}

//--------------------------------------------------------------------------------------------------
// ladownie animcji z obiektów C3DModel
void AnimationService::LoadAnimation( IC3DModel* c3dModel )
{
    if(c3dModel->GetMarkerList().size() == 0)
        return;

    m_pC3DModel = c3dModel;

    // extract number of animations				
    unsigned int numOfAnims = 0;


    // create animations
    Animation* animation = new Animation(c3dModel->GetMarkerList(), this);
    m_c3danimations.insert(make_pair(c3dModel, animation));	

    std::string animationName = c3dModel->GetName();
    animationName = animationName.substr(0, animationName.find_last_of("."));
    animationName.append(".amc");


    map<std::string, Animation*>::iterator i = m_animations.find(animationName);
    if (i == m_animations.end())
    {
        m_animationDisplayList.push_back(c3dModel->GetName()); 
    }
}

//--------------------------------------------------------------------------------------------------
void AnimationService::PlayAnimation(std::string animationName)
{
    timeline->setPlaying(false);
    timeline->setTime(0.0f);

    // znalezienie animacji w mapie
    map<std::string, Animation*>::iterator i = m_animations.find(animationName);
    
    // zatrzymanie aktualnej animacji
    if ( currentAnimation ) {
        currentAnimation->Stop();
        ClearCaller();
    }
    
    // jesli istnieje nowwa aniamacja - zgodna wybrana nazwa - pusc ja
    // jelsi nie wyczysc funktory - aktualne animajce.
    if (i != m_animations.end()){
        currentAnimation = i->second;
        i->second->Play();
    } 
    else{
        ClearCaller();
        currentAnimation = NULL;
        setLength(0.0f);
    }

    if (timeline && followTimeline) {
        timeline->addStream( timeline::StreamPtr(timeline::Stream::encapsulate(this)) );
    } else {
        OSG_WARN<<"ITimeline not found."<<std::endl;
    }

    m_pRenderService->DisableBone();

    PlayC3DAnimation(animationName);
}

//--------------------------------------------------------------------------------------------------
//Metoda na podstawie wybory z Diplaylisty - odpowiedniej animacji wybiera animacje dla markerów - obiekt C3DModel
// ktory powinnien posiada cidentyczna nazwe jak anmacje acm - z modelu
void AnimationService::PlayC3DAnimation(std::string name)
{
    std::string c3dName;

    c3dName = name.substr(0, name.find_last_of("."));
    c3dName.append(".c3d");

    widget->ClearMarkerList();

    if (c3dcurrentAnimation) {
        c3dcurrentAnimation->Stop();
    }

    map<IC3DModel*, Animation*>::iterator i;
    for(i = m_c3danimations.begin(); i != m_c3danimations.end(); i++)
    {
        if (i->first->GetName() == c3dName) 
        {
            m_pC3DModel = i->first;
            // ustawienie nowego modelu C3D do renderowania
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

    InitMarkerList();
}

//--------------------------------------------------------------------------------------------------
void AnimationService::GPUUpdateMesh()
{
    if(!m_pModel->GetSkeleton())
        return;

    Skeleton* skeleton = m_pModel->GetSkeleton();
    int boneCount = skeleton->m_pBoneList.size();

    // inicjalizacja daneych Uniform - tablicy macierzy kosci jak i bonspec tych kosci - na potrzebe wywo³ania i shadera 
    // Hardware animation
    if(!m_BoneUniform)
        m_BoneUniform = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "boneMatrices", boneCount);

    if(!m_BoneSpaceUniform)
        m_BoneSpaceUniform = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "boneSpaceMatrices", boneCount);

    for(int b = 0; b < boneCount; b++)
    {
        m_BoneUniform->setElement(b, *skeleton->m_pBoneList[b]->matrix);
    }
    for(int b = 0; b < boneCount; b++)
    {
        m_BoneSpaceUniform->setElement(b, skeleton->m_pBoneList[b]->bonespace);
    }

    std::vector<IMesh* > meshList = m_pModel->GetMeshList();

    // shader jest wykonywnay dla kazde VBO obiektu Geometry - co pozwala na wieksza elstycznosc.
    // np nie trzeba sie martwic o wierzcho³ki szkieletu. które moga sie reenderowac inaczej.
    // bezproblemowe powiazanie danych z shederaz atrybutami wrzycanymi do VBO z aplikacji
    for(int i = 0; i < meshList.size(); ++i)
    {
        meshList[i]->HardwareAnimation(m_BoneUniform, m_BoneSpaceUniform, m_HardwareAnimationShaderPath);
    } 
}

//--------------------------------------------------------------------------------------------------
void AnimationService::CPUUpdateMesh()
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
    return NULL;
}

//------------------------------------------------------------------------------
IWidget* AnimationService::getSettingsWidget()
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
        m_DisplayType = m_DisplayType | MESH;
    }
    else
    {
        m_pRenderService->DisableMesh();
        m_DisplayType = m_DisplayType ^ MESH;
    }

}

//--------------------------------------------------------------------------------------------------
void AnimationService::SetShowBone( bool showBone )
{
    if(showBone)
        m_DisplayType = m_DisplayType | BONE;
    else
    {
        m_pRenderService->DisableBone();
        m_DisplayType = m_DisplayType ^ BONE;
    }
}

//--------------------------------------------------------------------------------------------------
void AnimationService::SetShowMarker( bool showMarker )
{
    if(showMarker)
    {
        m_pRenderService->EnableMarker();
        m_DisplayType = m_DisplayType | MARKER;
    }
    else
    {
        m_pRenderService->DisableMarker();
        m_DisplayType = m_DisplayType ^ MARKER;
    }
}

//--------------------------------------------------------------------------------------------------
std::vector<std::string>* AnimationService::GetAnimationDisplayList()
{
    return &m_animationDisplayList;
}

//--------------------------------------------------------------------------------------------------
bool AnimationService::IsModelAnimation()
{
    if(currentAnimation)
        return true;

    return false;
}

//--------------------------------------------------------------------------------------------------
void AnimationService::InitMarkerList()
{
    if(!m_pC3DModel)
        return;

    // hack - skoro na raize i tak wyswietlamy po kolei numery markerów
    widget->AddMarkerTODisplayPathList(m_pC3DModel->GetMarkerList().size());
}

//--------------------------------------------------------------------------------------------------
void AnimationService::CreatingAndRenderMarkerPath(std::vector<int > markerSelectedList )
{
    m_pRenderService->CreatingAndRenderMarkerPath(m_pC3DModel, markerSelectedList);
}