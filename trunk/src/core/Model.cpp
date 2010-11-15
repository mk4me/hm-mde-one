#include "CorePCH.h"
#include "Model.h"

#include "Mesh.h"
#include "SkeletonNode.h"
#include <core/FModel.h>
#include <core/Matrix.h>


#define pPat osg::PositionAttitudeTransform*

using namespace osg;
using namespace std;

//--------------------------------------------------------------------------------------------------
Model::Model()
{
	m_geometry = new osg::Geometry();
    m_spSkeletonGeode = new osg::Geode();
    m_pAnimation = new::SkeletonAnimationList();

    m_pJoints = NULL;
    m_pSkeleton = NULL;
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
    m_pAnimation->m_SkeletonAnimationList.clear();

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
void Model::SetAnimation( SkeletonAnimationList* animation )
{
    m_pAnimation = animation;
}

//--------------------------------------------------------------------------------------------------
void Model::SetSkeleton(Skeleton* skeleton )
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
void Model::ApplyMaterial(std::vector<SMaterial>* materialList, std::wstring textureDir)
{
    if (materialList && materialList->size() > 0)
    {
        texmap::iterator i;
        if ((i = (*materialList)[0].texture_list.find(ETextureType::DIFFUSE_MAP)) != (*materialList)[0].texture_list.end())
        {
            Texture2D* tex = new Texture2D();

            wstring path = textureDir + L"/" + i->second;
            tex->setDataVariance(osg::Object::DYNAMIC); 
            osg::Image* klnFace = osgDB::readImageFile(string(path.begin(), path.end()));
            if (!klnFace) 
                return;

            tex->setImage(klnFace);

            StateSet* stateOne = new StateSet();
            stateOne->setTextureAttributeAndModes(0, tex, osg::StateAttribute::OVERRIDE|StateAttribute::ON);

            for(std::vector<IMesh* >::iterator it = m_meshList.begin(); it != m_meshList.end(); it++)
            { 
                if(dynamic_cast<Mesh*>(*it))
                    dynamic_cast<Mesh*>(*it)->setStateSet(stateOne);
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------
std::vector<IMesh* >& Model::GetMeshList()
{
    return m_meshList;
}

//--------------------------------------------------------------------------------------------------
Skeleton* Model::GetSkeleton()
{
    if(m_pSkeleton)
        return m_pSkeleton;

    return NULL;
}

//--------------------------------------------------------------------------------------------------
SkeletonAnimationList* Model::GetAnimation()
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
            CreateArrayOfJoints((SkeletonNode*)skeleton->asGroup()); 

            // CreateArrayOfJoints((SkeletonNode*)skeleton->getChild(0)->asGroup()); 

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

//--------------------------------------------------------------------------------------------------
void Model::DrawModelBone()
{
	if (m_spSkeletonGeode.valid())
		this->removeChild(m_spSkeletonGeode.get());

	// create new geode
	m_spSkeletonGeode = new osg::Geode();
	m_spSkeletonGeode->setName("skeleton_geode");

	// Drawing Bones


	Bone* bone = m_pSkeleton->m_pRootBone;
	int childcount = bone->child.size();

	for (int i = 0; i<childcount; i++)
	{
		this->DrawBone(bone->child[i], m_spSkeletonGeode);
	}

	this->addChild(m_spSkeletonGeode);

}

//--------------------------------------------------------------------------------------------------
void Model::DrawBone( Bone* bone, osg::Geode* geode)
{
    bool istrue = false;

    if(!strcmp(bone->name, "LeftHand"))
        istrue = true;

	geode->addDrawable(DrawLine(&osg::Vec3f((bone->parent)->positionx, (bone->parent)->positiony, (bone->parent)->positionz), &osg::Vec3f(bone->positionx,bone->positiony,bone->positionz), istrue));

	int childcount = bone->child.size();
	for (int i = 0; i<childcount; i++)
	{
		this->DrawBone(bone->child[i], geode);
	}
}

//--------------------------------------------------------------------------------------------------
osg::ref_ptr<osg::Geometry> Model::DrawTriangle(const osg::Vec3d* startPos, const osg::Vec3d* endPos, 
                                                const osg::Vec3d* vertexPos, const osg::Vec3d* startPos2, 
                                                const osg::Vec3d* endPos2, const osg::Vec3d* vertexPos2, bool isSelected)
{
    // draw actual bone
    

    // vertices
    osg::Vec3Array* vertices = new osg::Vec3Array();
    vertices->push_back(*startPos);
    vertices->push_back(*endPos);
    vertices->push_back(*vertexPos);
    vertices->push_back(*startPos2);
    vertices->push_back(*endPos2);
    vertices->push_back(*vertexPos2);

    // indices
    osg::DrawElementsUInt* line = new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON, 0);
    line->push_back(0);
    line->push_back(1);
    line->push_back(2);
    line->push_back(3);
    line->push_back(4);
    line->push_back(5);

    // set geometry data
    m_geometry->setVertexArray(vertices);
    m_geometry->addPrimitiveSet(line);

    // set colors
    osg::Vec4Array* colors = new osg::Vec4Array;
    if(isSelected)
    {
        colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
        colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
        colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
        colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
        colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
        colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
    }
    else
    {
        colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
        colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
        colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 0.0f));
        colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
        colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
        colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 0.0f));
    }


    osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType,4,4> *colorIndexArray;
    colorIndexArray = new osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType,4,4>;
    colorIndexArray->push_back(0);
    colorIndexArray->push_back(1);
    colorIndexArray->push_back(2);
    colorIndexArray->push_back(3);
    colorIndexArray->push_back(4);
    colorIndexArray->push_back(5);

    m_geometry->setColorArray(colors);
    m_geometry->setColorIndices(colorIndexArray);
    m_geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    return m_geometry;
}

//--------------------------------------------------------------------------------------------------
osg::ref_ptr<osg::Geometry> Model::DrawLine(const osg::Vec3f* startPos, const osg::Vec3f* endPos, bool isSelected)
{
    // draw actual bone
    osg::ref_ptr<osg::Geometry>  geometry = new osg::Geometry();

    // vertices
    osg::Vec3Array* vertices = new osg::Vec3Array();
    vertices->push_back(*startPos);
    vertices->push_back(*endPos);

    // indices
    osg::DrawElementsUInt* line = new osg::DrawElementsUInt(osg::PrimitiveSet::LINES, 0);
    line->push_back(0);
    line->push_back(1);

    // set geometry data
    geometry->setVertexArray(vertices);
    geometry->addPrimitiveSet(line);

    // set colors
    osg::Vec4Array* colors = new osg::Vec4Array;
    if(isSelected)
    {
        colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
        colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
    }
    else
    {
        colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
        colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType,4,4> *colorIndexArray;
    colorIndexArray = new osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType,4,4>;
    colorIndexArray->push_back(0);
    colorIndexArray->push_back(1);

    geometry->setColorArray(colors);
    geometry->setColorIndices(colorIndexArray);
    geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    return geometry;
}

//--------------------------------------------------------------------------------------------------
osg::ref_ptr<osg::Geode> Model::GetSkeletonGeode()
{
    return m_spSkeletonGeode;
}