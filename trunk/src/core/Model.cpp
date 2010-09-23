#include "Model.h"

#include "Mesh.h"
#include "SkeletonNode.h"



//--------------------------------------------------------------------------------------------------
Model::Model()
{

}

//--------------------------------------------------------------------------------------------------
Model::~Model()
{
    Clear();
}

//--------------------------------------------------------------------------------------------------
void Model::Clear()
{
    m_meshList.clear();

    m_pJoints = NULL;
    m_pSkeleton = NULL;
    m_pAnimation = NULL;
}

//--------------------------------------------------------------------------------------------------
void Model::AddMesh( IMesh* mesh )
{
    m_meshList.push_back(mesh);
}

//--------------------------------------------------------------------------------------------------
void Model::SetAnimation( SFAnimation* animation )
{
    m_pAnimation = animation;
}

//--------------------------------------------------------------------------------------------------
void Model::SetSkeleton( SSkeleton* skeleton )
{
    m_pSkeleton = skeleton;
}

//--------------------------------------------------------------------------------------------------
IMesh* Model::GetMesh( int i /*= 0*/ )
{
    if(m_meshList.size() > i)
        return m_meshList[i];

    return NULL;
}

//--------------------------------------------------------------------------------------------------
void Model::InicializeMesh()
{
	for(std::vector<IMesh* >::iterator it = m_meshList.begin(); it != m_meshList.end(); it++)
	{
		(*it)->Inicialize();
	}
}

//--------------------------------------------------------------------------------------------------
std::vector<IMesh* >& Model::GetMeshList()
{
    return m_meshList;
}

//--------------------------------------------------------------------------------------------------
SSkeleton* Model::GetSkeleton()
{
    if(m_pSkeleton)
        return m_pSkeleton;

    return NULL;
}

//--------------------------------------------------------------------------------------------------
SFAnimation* Model::GetAnimation()
{
    if(m_pAnimation)
        return m_pAnimation;

    return NULL;
}

//--------------------------------------------------------------------------------------------------
void Model::SetJoints( std::vector<ISkeletonNode* >* joints )
{
    m_pJoints = joints;
}

//--------------------------------------------------------------------------------------------------
std::vector<ISkeletonNode* >* Model::GetJoints()
{

    // find skeleton
    int childNum = this->getNumChildren();
    for (int i = 0; i < childNum; ++i)
    {		
        osg::Group* group = dynamic_cast<SkeletonNode*>(this->getChild(i));
        if (group)
        {
            //////////////////////////////////////////////////////////////////////////
            // get skeleton and set pointer at him as member
            osg::ref_ptr<osg::Group> skeleton = this->getChild(i)->asGroup();

            // create vector of pointers at bones
            m_pJoints = new std::vector<ISkeletonNode*>();
            CreateArrayOfJoints((SkeletonNode*)skeleton->getChild(0)->asGroup()); 

            return m_pJoints;
        }
    }
    
    return NULL;
}

//--------------------------------------------------------------------------------------------------
void Model::CreateArrayOfJoints(SkeletonNode* bone)
{
    std::string name = bone->getName();

    m_pJoints->push_back(bone);

    for (unsigned int b = 0; b < bone->getNumChildren(); ++b)
        if (dynamic_cast<SkeletonNode*>(bone->getChild(b)))
            CreateArrayOfJoints((SkeletonNode*)bone->getChild(b));
}

//--------------------------------------------------------------------------------------------------
void* Model::GetSkeletonGroup()
{
    int childNum = this->getNumChildren();
    for (int i = 0; i < childNum; ++i)
    {		
        osg::Group* group = dynamic_cast<SkeletonNode*>(this->getChild(i));
        if (group)
        {
            //////////////////////////////////////////////////////////////////////////
            // get skeleton and set pointer at him as member
            osg::ref_ptr<osg::Group> skeleton = this->getChild(i)->asGroup();
            return (void*)skeleton;
        }
    }

    return NULL;
}