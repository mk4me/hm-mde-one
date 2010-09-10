#include "AnimationService.h"

#include <iostream>
#include <osgViewer/Scene>
#include <osg/Group>
#include <osg/Geode>
#include <osg/NodeVisitor>

#include "../OsgExtensions/Model.h"
#include "../OsgExtensions/Mesh.h"
#include "../OsgExtensions/Vec3.h"
#include "Animation.h"


#include "../OsgExtensions/SkeletonNode.h"



using namespace std;
using namespace osg;

M_DECLARED_CLASS(AnimationService, kCLASSID_AnimationService);


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
, m_selectedAnimatonName("")
{
}

//--------------------------------------------------------------------------------------------------
AnimationService::~AnimationService(void)
{
	Clear();
}

//--------------------------------------------------------------------------------------------------
AsyncResult AnimationService::OnTick(double delta)
{ 
	if (m_pAnimation)
	{
		// update animation
		(*m_pAnimation)(delta);
        UpdateSkeleton();
        RecalculateChanges();
        UpdateMesh();

		// call functions that are to call every animation frame
		for (std::vector<ISimpleOneArgFunctor<double>*>::iterator i = m_functionsToCall.begin(); i != m_functionsToCall.end(); ++i)
			(**i)(delta);

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
AsyncResult AnimationService::OnAdded()
{
    std::cout << "AnimationService ADDED!" << std::endl; 
    return AsyncResult_Complete; 
}

//--------------------------------------------------------------------------------------------------
void AnimationService::SetScene(osgViewer::Scene* scene)
{
    std::cout << "AnimationService: Scene added" << std::endl; 
    m_pScene = scene; 
}

//--------------------------------------------------------------------------------------------------
void AnimationService::SetSelectedAnimationName(std::string& name)
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
void AnimationService::LoadAnimation( NewModel* model )
{
    Clear();

    m_pModel = model;

    // find skeleton
    int childNum = m_pModel->getNumChildren();
    for (int i = 0; i < childNum; ++i)
    {		
        Group* group = dynamic_cast<SkeletonNode*>(m_pModel->getChild(i));
        if (group)
        {
            //////////////////////////////////////////////////////////////////////////
            // get skeleton and set pointer at him as member
            m_skeleton = m_pModel->getChild(i)->asGroup();

            // create vector of pointers at bones
            m_pJoints = new vector<SkeletonNode*>();
            CreateArrayOfJoints((SkeletonNode*)m_skeleton->getChild(0)->asGroup());

            m_numOfBones = m_pJoints->size();
            m_numOfBones += 1; // because we don't added first node (root_of_Skeleton)

            // get 'global' (? - not another transformations but entire propagated from root) 
            // transformations of bones
            m_pActualBones = new STransform [m_numOfBones];
            UpdateSkeleton();

            // save initial values
            m_pInitialBones = new STransform [m_numOfBones];
            memcpy(m_pInitialBones, m_pActualBones, sizeof(STransform) * m_numOfBones);

            //////////////////////////////////////////////////////////////////////////
            // handle animations

            // extract number of animations				
            unsigned int numOfAnims = 0;
            vector<string> names;
            for (vector<SkeletonNode*>::iterator i = m_pJoints->begin(); i != m_pJoints->end(); ++i)
            {
                if ((*i)->GetNumOfAnimations() > numOfAnims)
                {					
                    for (unsigned int j = numOfAnims; j < (*i)->GetNumOfAnimations(); ++j)
                        names.push_back((*(*i)->GetAnimations())[j].get()->getName());

                    numOfAnims = (*i)->GetNumOfAnimations();
                }
            }

            int counting = m_pJoints->size();

            // create animations
            for (unsigned int i = 0; i < numOfAnims; ++i)
            {
                Animation* animation = new Animation(m_pJoints, i);
                m_animations.insert(make_pair(names[i], animation));	
            }

            // TODO:
            // AFAIK - skeleton may/should be only one...
            break;
        }
    }	
}

//--------------------------------------------------------------------------------------------------
// creates array of pointers at bones sorted by id
void AnimationService::CreateArrayOfJoints(SkeletonNode* bone)
{
    std::string name = bone->getName();

    m_pJoints->push_back(bone);

    for (unsigned int b = 0; b < bone->getNumChildren(); ++b)
        if (dynamic_cast<SkeletonNode*>(bone->getChild(b)))
            CreateArrayOfJoints((SkeletonNode*)bone->getChild(b));
}

//--------------------------------------------------------------------------------------------------
// update skeleton
void AnimationService::UpdateSkeleton()
{
    pPat skeleton = dynamic_cast<SkeletonNode*>(m_skeleton.get());
    if (skeleton)
    {
        for (unsigned int b = 0; b < skeleton->getNumChildren(); ++b)
            if (dynamic_cast<SkeletonNode*>(skeleton->getChild(b)) )
                UpdateBone((pPat)skeleton->getChild(b));
    }
}

//--------------------------------------------------------------------------------------------------
// creates arrays of pointers at bones sorted by id
void AnimationService::UpdateBone(osg::PositionAttitudeTransform* bone)
{
    unsigned int pId = ((SkeletonNode*)bone->getParent(0))->GetId();
    unsigned int boneId = ((SkeletonNode*)bone)->GetId();

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
        if (dynamic_cast<SkeletonNode*>(bone->getChild(b)))
            UpdateBone((pPat)bone->getChild(b));

}

//--------------------------------------------------------------------------------------------------
void AnimationService::PlayAnimation(std::string animationName)
{
    map<std::string, Animation*>::iterator i = m_animations.find(animationName);

    if (i != m_animations.end())
    {
        i->second->Play();
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
    std::vector<NewMesh*> meshList = m_pModel->GetMeshList();
    int countHandv = 0;

    for(std::vector<NewMesh*>::iterator it = meshList.begin(); it != meshList.end(); it++)
    {
        NewMesh* mesh = *it;
        SSkin* skin = NULL;

        if (mesh && (skin = mesh->skin))
        {
            // update vertices using skin
            for (int i = 0; i < skin->n; i++)
            {
                // vertice
                SSkinnedVertice* vertice = &skin->skinned_vertices[i];

                // act position of vertice
                Vec3d actPos(mesh->m_pRootVerts[vertice->vert_id]._v[0], 
                    mesh->m_pRootVerts[vertice->vert_id]._v[1], mesh->m_pRootVerts[vertice->vert_id]._v[2]);

                // normal
                Vec3d normal(mesh->m_pRootVertNormals[vertice->vert_id]._v[0], 
                    mesh->m_pRootVertNormals[vertice->vert_id]._v[1], mesh->m_pRootVertNormals[vertice->vert_id]._v[2]);


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
                mesh->m_pVerts[vertice->vert_id]._v[0] = change.x();
                mesh->m_pVerts[vertice->vert_id]._v[1] = change.y();
                mesh->m_pVerts[vertice->vert_id]._v[2] = change.z();

                // update of normal 
                mesh->m_pVertNormals[vertice->vert_id]._v[0] = nchange.x();
                mesh->m_pVertNormals[vertice->vert_id]._v[1] = nchange.y();
                mesh->m_pVertNormals[vertice->vert_id]._v[2] = nchange.z();
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