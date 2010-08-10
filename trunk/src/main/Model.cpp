#include "Model.h"

#include <osg/Geometry>
#include <algorithm>
#include <math.h>

#include "ServiceManager.h"
#include "ObjectService.h"

#include "../OsgExtensions/CustomGroup.h"
#include "../OsgExtensions/SkeletonNode.h"

#include <osg/Texture2D>

#include <osg/Material>
#include <osg/PolygonOffset>
#include <osg/PolygonMode>
#include <osg/LineStipple>

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osg/PolygonOffset>
#include <osg/PolygonMode>

#include <osg/Notify>
#include <osg/Group>
#include <osg/observer_ptr>


#include "ServiceManager.h"
#include "ToolboxMain.h"
#include "ObjectService.h"


#include "../osgDBPlugin/FileChunkReader.h"

// helper - this name is quite long...
#define pPat osg::PositionAttitudeTransform*

#define POSITION 0
#define NORMALS  1

using namespace osg;
using namespace std;


//--------------------------------------------------------------------------------------------------
void itoa(int value, std::string& buf, int base)
{
    int i = 30;

    buf = "";

    for(; value && i ; --i, value /= base) buf = "0123456789abcdef"[value % base] + buf;

}

//--------------------------------------------------------------------------------------------------
Model::Model(const std::string &fileName, bool visible):
  _joints(NULL)
, _initialBones(NULL) 
, _lastPlayedAnimation("")
, _actualBones(NULL)
{
    InicializeMesh(fileName, visible);

    _pScene = ServiceManager::GetInstance()->GetSystemServiceAs<ObjectService>(); 

    if (_root.valid())
    {
        // find skeleton
        int childNum = _root->getNumChildren();
        for (int i = 0; i < childNum; ++i)
        {		
            Group* group = dynamic_cast<SkeletonNode*>(_root->getChild(i));
            if (group)
            {
                //////////////////////////////////////////////////////////////////////////
                // get skeleton and set pointer at him as member
                _skeleton	= _root->getChild(i)->asGroup();

                // create vector of pointers at bones
                _joints		= new vector<SkeletonNode*>();
                CreateArrayOfJoints((SkeletonNode*)_skeleton->getChild(0)->asGroup());

                _numOfBones	= _joints->size();
                _numOfBones += 1; // because we don't added first node (root_of_Skeleton)

                // get 'global' (? - not another transformations but entire propagated from root) 
                // transformations of bones
                _actualBones  = new STransformation [_numOfBones];
                UpdateSkeleton();

                // save initial values
                _initialBones = new STransformation [_numOfBones];
                memcpy(_initialBones, _actualBones, sizeof(STransformation) * _numOfBones);

                //////////////////////////////////////////////////////////////////////////
                // handle animations

                // extract number of animations				
                unsigned int numOfAnims = 0;
                vector<string> names;
                for (vector<SkeletonNode*>::iterator i = _joints->begin(); i != _joints->end(); ++i)
                {
                    if ((*i)->GetNumOfAnimations() > numOfAnims)
                    {					
                        for (unsigned int j = numOfAnims; j < (*i)->GetNumOfAnimations(); ++j)
                            names.push_back((*(*i)->GetAnimations())[j].get()->getName());

                        numOfAnims = (*i)->GetNumOfAnimations();
                    }
                }

                int counting = _joints->size();

                // create animations
                for (unsigned int i = 0; i < numOfAnims; ++i)
                {
                    Animation* animation = new Animation(_joints, i);
                    _animations.insert(make_pair(names[i], animation));	
                }

                // TODO:
                // AFAIK - skeleton may/should be only one...
                break;
            }
        }	
    }
}

//--------------------------------------------------------------------------------------------------
Model::Model(bool visible, std::vector<SkeletonNode*>* joints, STransformation* initialBones, 
                                     std::string _lastPlayedAnimation, STransformation* actualBones, SSkeleton* skeleton, 
                                     SFAnimation* animation, SVertice* meshBuffer):
  _joints(joints)
, _initialBones(initialBones)
, _lastPlayedAnimation(_lastPlayedAnimation)
, _actualBones(actualBones)
{

    InicializeMesh(meshBuffer, visible);

    ref_ptr<CCustomGroup> root = new CCustomGroup();

    // meshBuffer
    SModelData* fmesh = new SModelData();
    root->setFMesh(fmesh);

    // kosci
    root->setSkeleton(skeleton);
    root->setAnimations(animation);

    _root = root;
}

//--------------------------------------------------------------------------------------------------
Model::~Model()
{
    // it's all i suppose...
    if (_root.valid())
    {
        for (int i = _root->getNumParents() - 1; i >= 0; --i)
        {
            _root->getParent(i)->removeChild(_root);
        }
    }

    if (_meshBufferList.size() > 0)
        _meshBufferList.clear();

    _meshRootBufferList.clear();
    _meshes.clear();


    if (_joints)
        delete _joints;
    _joints = NULL;

    if (_initialBones)
        delete [] _initialBones;
    _initialBones = NULL;

    if (_actualBones)
        delete [] _actualBones;
    _actualBones = NULL;

    _animations.clear();
    ((CCustomGroup*)_root.get())->Clear();

    _pScene = NULL;
}

//--------------------------------------------------------------------------------------------------
// recalculates changes of skeleton
void Model::RecalculateChanges()
{
	for (unsigned int b = 0; b < (_numOfBones + 1); ++b)
	{
		// cross product
		Vec3d	cross	 = _initialBones[b].bone ^ _actualBones[b].bone;	
		// cos of angle between old and new bone
		double  cosangle = (_initialBones[b].bone * _actualBones[b].bone) / (_initialBones[b].bone.length() * _actualBones[b].bone.length());
		// angle between old and new bone
		double	angle    = acos(cosangle);	// acos of dot product div by lenths
		
		// construct bone rotation...
		_actualBones[b].rotation	= Quat(angle, cross);
		// ...transformation
		_actualBones[b].translation = _actualBones[b].point - _initialBones[b].point;
	}
}

//--------------------------------------------------------------------------------------------------
// update skeleton
void Model::UpdateSkeleton()
{
	pPat skeleton = dynamic_cast<SkeletonNode*>(_skeleton.get());
	if (skeleton)
    {
		for (unsigned int b = 0; b < skeleton->getNumChildren(); ++b)
			if (dynamic_cast<SkeletonNode*>(skeleton->getChild(b)) )
				UpdateBone((pPat)skeleton->getChild(b));
	}
}

//--------------------------------------------------------------------------------------------------
void Model::Update()
{
    // TODO: do rpzemyœlenia, Applymaterial moze byæ w Load Model - wtedy bêdizemy wywo³ywaæ Loda Model przy Open - w toolboxie,
    // zamiast Update. Albo tuaj, w tedy mamy kontrlo nad materia³ami nie wazne czy jest robiony UpadeteMesh czy LoadModel
    UpdateBones();

    if(!_isModelLoaded)
    {
        LoadModel();
        _isModelLoaded = true;
    }
    else
        UpdateMesh();


    if(_pScene->GetViewModelFlag() & ObjectService::MATERIAL)
        ApplyMaterial(0, _materialId, true);

    if(_pScene->GetViewModelFlag() & ObjectService::WIREFRAME)
        UpdateWireFrame();
}

//--------------------------------------------------------------------------------------------------
// creates arrays of pointers at bones sorted by id
void Model::UpdateBone(osg::PositionAttitudeTransform* bone)
{
	unsigned int pId = ((SkeletonNode*)bone->getParent(0))->GetId();
    unsigned int boneId = ((SkeletonNode*)bone)->GetId();

    std::string name = bone->getName();


    osg::Vec3d ppos = pId ? _actualBones[pId].position : ((pPat)_skeleton.get())->getPosition();
    osg::Quat  prot = pId ? _actualBones[pId].attitude : ((pPat)_skeleton.get())->getAttitude();

    osg::Vec3d bpos = prot * bone->getPosition() + ppos;
    osg::Quat  brot = bone->getAttitude() * prot;

    STransformation tr;
    tr.attitude	= brot;
    tr.position	= bpos;

    tr.bone		= bpos - ppos;
    tr.point	= ppos;

    if (bone->getNumChildren() > 0)
        tr.hasChild = true;
    else
        tr.hasChild = false;

    _actualBones[boneId] = tr;

    for (unsigned int b = 0; b < bone->getNumChildren(); ++b)
        if (dynamic_cast<SkeletonNode*>(bone->getChild(b)))
            UpdateBone((pPat)bone->getChild(b));

}

//--------------------------------------------------------------------------------------------------
// update mesh
bool Model::UpdateBones()
{
    int countHandv = 0;
     for (unsigned int j = 0; j < (((CCustomGroup*)_root.get())->getNumOfMeshes()); ++j)

     {
         SMesh* mesh = ((CCustomGroup*)_root.get())->getMesh(j);
         SSkin* skin = NULL;

         if(_pScene->GetViewModelFlag() & ObjectService::BONE)
             UpdateSkeletonMesh();

         if (mesh && (skin = mesh->modifier->skin))
         {
             // updates skeleton
             UpdateSkeleton();
             // recalculates changes of skeleton
             RecalculateChanges();

             SVertice* vData = _meshRootBufferList[j];
             
             // update vertices using skin
             for (int i = 0; i < skin->n; i++)
             {
                 // vertice
                 SSkinnedVertice* vertice = &skin->skinned_vertices[i];

                 // act position of vertice
                 Vec3d actPos(vData[vertice->vert_id].position[0], 
                     vData[vertice->vert_id].position[1], vData[vertice->vert_id].position[2]);

                 // normal
                 Vec3d normal(vData[vertice->vert_id].normal[0], 
                     vData[vertice->vert_id].normal[1], vData[vertice->vert_id].normal[2]);



                 // for every affecting bone
                 for (int b = 0; b < vertice->n; b++)
                 {
                     int boneID = vertice->bones[b].boneID;
                     if(_actualBones[boneID].hasChild)
                     {
                         boneID++;  // rotacje bierzemy z dziecka. Z konca koœci która jest pocz¹tkiem nastêpnej.
                     }

                     _tempVectors[b][POSITION] =	_actualBones[boneID].rotation 
                         * (actPos - _actualBones[boneID].point									
                         + _actualBones[boneID].translation)									
                         + _actualBones[boneID].point;

                     _tempVectors[b][NORMALS]  = _actualBones[vertice->bones[b].boneID].rotation * normal;

                 }

                 // get output point
                 Vec3d change, nchange;
                 for (int b = 0; b < vertice->n; b++)
                 {
                     change  += _tempVectors[b][POSITION] * vertice->bones[b].weight;
                     nchange += _tempVectors[b][NORMALS]  * vertice->bones[b].weight;
                 }


                 // update of vertex
                 _meshBufferList[j][vertice->vert_id].position[0] = change.x();
                 _meshBufferList[j][vertice->vert_id].position[1] = change.y();
                 _meshBufferList[j][vertice->vert_id].position[2] = change.z();

                 // update of normal 
                 _meshBufferList[j][vertice->vert_id].normal[0] = nchange.x();
                 _meshBufferList[j][vertice->vert_id].normal[1] = nchange.y();
                 _meshBufferList[j][vertice->vert_id].normal[2] = nchange.z();
             }
         }
     }

     return true;
}

//--------------------------------------------------------------------------------------------------
// creates array of pointers at bones sorted by id
void Model::CreateArrayOfJoints(SkeletonNode* bone)
{

    std::string name = bone->getName();

	_joints->push_back(bone);

	for (unsigned int b = 0; b < bone->getNumChildren(); ++b)
		if (dynamic_cast<SkeletonNode*>(bone->getChild(b)))
			CreateArrayOfJoints((SkeletonNode*)bone->getChild(b));
}

//--------------------------------------------------------------------------------------------------
// get animation
Animation*	Model::GetAnimation(std::string& name)
{
	map<std::string, Animation*>::iterator i = _animations.find(name);

	if (i != _animations.end())
	{
		_lastPlayedAnimation = name;
		return i->second;
	}
	else
		return NULL;
}

//--------------------------------------------------------------------------------------------------
void Model::DrawBone(pPat bone, const osg::Vec3d* parentPos, const osg::Quat* parentRot, osg::Geode* geode)
{
	osg::Vec3d bpos = (*parentRot) * bone->getPosition() + *parentPos;
	osg::Quat  brot = bone->getAttitude() * (*parentRot);


    float length = (*parentPos - bpos).length();
    float distanceHightToArm = length/8;
    float distanceWidthToArm = length/12;

    osg::Vec3d boneNodeParent;
    osg::Vec3d boneNodeChild;
    osg::Vec3d boneNodeLeftArm;
    osg::Vec3d boneNodeRightArm;

    osg::Vec3d boneNodeFrontArm;
    osg::Vec3d boneNodeBackArm;
	osg::Vec3d result;
	osg::Vec3d punkt;

    boneNodeParent = osg::Vec3(0.f, 0.f, 0.f);
    boneNodeChild = bone->getPosition(); //osg::Vec3(length, 0.f, 0.f); 


	boneNodeLeftArm = osg::Vec3(distanceHightToArm, -distanceWidthToArm, 0.f);
    boneNodeRightArm = osg::Vec3(distanceHightToArm, distanceWidthToArm, 0.f);
 
    boneNodeFrontArm = osg::Vec3(distanceHightToArm, 0.f, -distanceWidthToArm);
    boneNodeBackArm = osg::Vec3(distanceHightToArm, 0.f, distanceWidthToArm);

    boneNodeParent = (*parentRot) * boneNodeParent + *parentPos;
    boneNodeChild = (*parentRot) * boneNodeChild + *parentPos;

    boneNodeLeftArm = (*parentRot) * boneNodeLeftArm + *parentPos;
    boneNodeRightArm = (*parentRot) * boneNodeRightArm + *parentPos;

    boneNodeFrontArm = (*parentRot) * boneNodeFrontArm + *parentPos;
    boneNodeBackArm = (*parentRot) * boneNodeBackArm + *parentPos;

    bool isSelected = false;
    if(_selectedGroupName == bone->getName())
    {
        isSelected = true;
    }

    geode->addDrawable(DrawTriangle(&boneNodeParent, &boneNodeLeftArm, &boneNodeFrontArm, &boneNodeRightArm, &boneNodeBackArm, &boneNodeLeftArm, isSelected));
	geode->addDrawable(DrawTriangle(&boneNodeChild, &boneNodeLeftArm, &boneNodeFrontArm, &boneNodeRightArm, &boneNodeBackArm, &boneNodeLeftArm, isSelected));


    // recursively draw other bones
    for (unsigned int b = 0; b < bone->getNumChildren(); ++b)
        if (dynamic_cast<SkeletonNode*>(bone->getChild(b)))
            DrawBone((pPat)bone->getChild(b), &bpos, &brot, geode);
}

//--------------------------------------------------------------------------------------------------
void Model::UpdateSkeletonMesh()
{
	// remove old geode
	if (_skeletonGeode.valid())
		_root->removeChild(_skeletonGeode.get());

	// create new geode
	_skeletonGeode = new osg::Geode();
	_skeletonGeode->setName("skeleton_geode");

	// recursively draw skeleton
	pPat root_bone = (pPat)(_skeleton.get());

	for (unsigned int b = 0; b < root_bone->getNumChildren(); ++b)
		if (dynamic_cast<SkeletonNode*>(root_bone->getChild(b)))
			DrawBone((pPat)root_bone->getChild(b), &root_bone->getPosition(), &root_bone->getAttitude(), _skeletonGeode);

	_root->addChild(_skeletonGeode);
}

//--------------------------------------------------------------------------------------------------
void Model::UpdateWireFrame()
{
    // remove old geode
    if (_wireFrameGeode.valid())
        _root->removeChild(_wireFrameGeode.get());

    // create new geode
    _wireFrameGeode = new osg::Geode();
    _wireFrameGeode->setName("wireFrame_geode");

    DrawWireFrame(_wireFrameGeode);

    _root->addChild(_wireFrameGeode);
}

//--------------------------------------------------------------------------------------------------
osg::ref_ptr<osg::Geometry> Model::DrawLine(const osg::Vec3d* startPos, const osg::Vec3d* endPos)
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
    colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
    colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

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
osg::ref_ptr<osg::Geometry> Model::DrawTriangle(const osg::Vec3d* startPos, const osg::Vec3d* endPos, 
                                                            const osg::Vec3d* vertexPos, const osg::Vec3d* startPos2, 
                                                            const osg::Vec3d* endPos2, const osg::Vec3d* vertexPos2, bool isSelected)
{
	// draw actual bone
	osg::ref_ptr<osg::Geometry>  geometry = new osg::Geometry();

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
	geometry->setVertexArray(vertices);
	geometry->addPrimitiveSet(line);

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

	geometry->setColorArray(colors);
	geometry->setColorIndices(colorIndexArray);
	geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	return geometry;
}
//--------------------------------------------------------------------------------------------------
void Model::SelectGroup( std::string nameGroup)
{
    _selectedGroupName = nameGroup;
}

//--------------------------------------------------------------------------------------------------
size_t Model::GetVertexSize()
{
    size_t vertexSize = 0; 
    size_t semanticCount = ((CCustomGroup*)_root.get())->getMesh()->mesh_buffer->element_count; 
    for(size_t i=0; i<semanticCount; ++i)
    {
        size_t size = 0; 
        switch(((CCustomGroup*)_root.get())->getMesh()->mesh_buffer->vertex_format[i].type)
        {
        case Position: 
            size = 3 * FLOAT_SIZE; 
            break; 
        case Normal: 
            size = 3 * FLOAT_SIZE; 
            break; 
        case Binormal: 
            size = 3 * FLOAT_SIZE; 
            break; 
        case Tangent: 
            size = 3 * FLOAT_SIZE; 
            break; 
        case TexCoord0: 
            size = 2 * FLOAT_SIZE; 
            break; 
        default:
            size = 0; 
        }
        vertexSize += size; 
    }
    return vertexSize; 
}

//--------------------------------------------------------------------------------------------------
// Loads model from file 
void Model::InicializeMesh(const std::string& fileName, bool visible)
{
    _root = NULL;
    _materialId = -1;
    _properlyLoaded = false;

    _meshBufferList.clear();
    _meshRootBufferList.clear();

    _isModelLoaded = false;

    // calls external function loading model <- look into osgDBPlugin
    ref_ptr<osg::Node> ret = osgDB::readNodeFile(fileName);

    if (dynamic_cast<CCustomGroup*>(ret.get()))
    {
        if (ret.valid() && (_root = ret->asGroup())) // this assign is meant to be here
        {
            //     RemoveGeodes();
            // TODO: - materialy dzialaja na "slowo honoru"
            // apply proper material
            vector<vector<int> >* setList = ((CCustomGroup*)_root.get())->getMaterialSetList();
            if (setList && setList->size() && (*setList)[0].size())
                _materialId = (*setList)[0][0];

            // Nowa zmiana wczytywania mesha.
            // TODO: zoptymalizowaæ potem - std::vector nie robi g³êbokiej kopi jeœli ich elementem jest wskaŸnik - dlatego taki ba³agan
            // spróbowaæ skorzystac s shared_ptr
            int numberOfElements = 0;
            int numberOfMeshes = ((CCustomGroup*)_root.get())->getNumOfMeshes();

            if(numberOfMeshes > 0)
                _vertexSize = GetVertexSize();


            //  za³adownie danych z wszsytkich meshy
            for(int i = 0; i<numberOfMeshes; i++)
            {
                if(((CCustomGroup*)_root.get())->getMesh(i) != NULL)
                {
                    numberOfElements = ((CCustomGroup*)_root.get())->getMesh(i)->mesh_buffer->n / _vertexSize;

                    SVertice* meshBuffer = new SVertice[numberOfElements];
                    SVertice* meshBufferRoot =  new SVertice[numberOfElements];


                    size_t semanticCount = ((CCustomGroup*)_root.get())->getMesh(i)->mesh_buffer->element_count; 
                    int indexComplexBuffor = 0;

                    for(int k = 0; k<numberOfElements; k++)
                    {
                        for(size_t j=0; j<semanticCount; j++)
                        {
                            size_t size = 0; 
                            switch(((CCustomGroup*)_root.get())->getMesh(i)->mesh_buffer->vertex_format[j].type)
                            {
                            case Position: 
                                size = 3 * FLOAT_SIZE;

                                memcpy (meshBuffer[k].position, &((CCustomGroup*)_root.get())->getMesh(i)->mesh_buffer->buffer[indexComplexBuffor], size);
                                memcpy (meshBufferRoot[k].position, &((CCustomGroup*)_root.get())->getMesh(i)->mesh_buffer->buffer[indexComplexBuffor], size);
                                indexComplexBuffor += size;
                                break; 

                            case Normal: 
                                size = 3 * FLOAT_SIZE; 

                                memcpy (meshBuffer[k].normal, &((CCustomGroup*)_root.get())->getMesh(i)->mesh_buffer->buffer[indexComplexBuffor], size);
                                memcpy (meshBufferRoot[k].normal, &((CCustomGroup*)_root.get())->getMesh(i)->mesh_buffer->buffer[indexComplexBuffor], size);
                                indexComplexBuffor += size;
                                break; 

                            case Binormal: 
                                size = 3 * FLOAT_SIZE; 

                                memcpy (meshBuffer[k].binormal, &((CCustomGroup*)_root.get())->getMesh(i)->mesh_buffer->buffer[indexComplexBuffor], size);
                                memcpy (meshBufferRoot[k].binormal, &((CCustomGroup*)_root.get())->getMesh(i)->mesh_buffer->buffer[indexComplexBuffor], size);
                                indexComplexBuffor += size;
                                break; 

                            case Tangent: 
                                size = 3 * FLOAT_SIZE; 

                                memcpy (meshBuffer[k].tangent, &((CCustomGroup*)_root.get())->getMesh(i)->mesh_buffer->buffer[indexComplexBuffor], size);
                                memcpy (meshBuffer[k].tangent, &((CCustomGroup*)_root.get())->getMesh(i)->mesh_buffer->buffer[indexComplexBuffor], size);
                                indexComplexBuffor += size;
                                break; 

                            case TexCoord0: 
                                size = 2 * FLOAT_SIZE; 

                                memcpy (meshBuffer[k].texture_vertex, &((CCustomGroup*)_root.get())->getMesh(i)->mesh_buffer->buffer[indexComplexBuffor], size);
                                memcpy (meshBufferRoot[k].texture_vertex, &((CCustomGroup*)_root.get())->getMesh(i)->mesh_buffer->buffer[indexComplexBuffor], size);
                                indexComplexBuffor += size;
                                break; 

                            default:
                                size = 0; 
                            }
                        } 
                    }

                    _meshBufferList.push_back(meshBuffer);
                    _meshRootBufferList.push_back(meshBufferRoot);

                }
                _properlyLoaded = true;
            }

        }
    }

    _pScene = ServiceManager::GetInstance()->GetSystemServiceAs<ObjectService>(); 
}

//--------------------------------------------------------------------------------------------------
// Manually creates the model 
void Model::InicializeMesh(SVertice* meshBuffer,  bool visible)
{
    _root= NULL;
    _materialId = -1;
    _properlyLoaded = false;

    _meshBufferList.clear();
    _meshRootBufferList.clear();
}

//--------------------------------------------------------------------------------------------------
// draws normal vectors
void Model::DrawNormals(float size)
{
    if (_meshes.size())
    {

        for (unsigned int i = 0; i < (((CCustomGroup*)_root.get())->getNumOfMeshes()); ++i)
        {
            SMesh* mesh = ((CCustomGroup*)_root.get())->getMesh(i);

            if (mesh)
            {
                ref_ptr<Geode> geode = new Geode();

                int numOfVertices= mesh->mesh_buffer->n / _vertexSize;


                SVertice* vData = _meshBufferList[i];

                // for every vertice
                for (int v = 0; v < numOfVertices; ++v)
                {
                    ref_ptr<Geometry> geometry = new Geometry();

                    // vertices
                    Vec3Array* vertices = new Vec3Array();
                    vertices->push_back(Vec3(	vData[v].position[0], vData[v].position[1], vData[v].position[2]));
                    vertices->push_back(Vec3(	vData[v].position[0] + size * vData[v].normal[0], 
                        vData[v].position[1] + size * vData[v].normal[1], 
                        vData[v].position[2] + size * vData[v].normal[2]));

                    // indices
                    osg::DrawElementsUInt* line = new osg::DrawElementsUInt(osg::PrimitiveSet::LINES, 0);
                    line->push_back(0);
                    line->push_back(1);

                    // set geometry data
                    geometry->setVertexArray(vertices);
                    geometry->addPrimitiveSet(line);

                    // set colors
                    osg::Vec4Array* colors = new osg::Vec4Array;
                    colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
                    colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

                    osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType,4,4> *colorIndexArray;
                    colorIndexArray = new osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType,4,4>;
                    colorIndexArray->push_back(0);
                    colorIndexArray->push_back(1);

                    geometry->setColorArray(colors);
                    geometry->setColorIndices(colorIndexArray);
                    geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

                    geode->addDrawable(geometry);
                }

                geode->setName("normals");

                _root->addChild(geode);
            }
        }	
    }
}

//--------------------------------------------------------------------------------------------------
// draws WireFrame
void Model::DrawWireFrame(osg::Geode* geode)
{
    for (unsigned int i = 0; i < (((CCustomGroup*)_root.get())->getNumOfMeshes()); ++i)
    {
        ref_ptr<Geometry>	geometry = new Geometry();

        SMesh* mesh = ((CCustomGroup*)_root.get())->getMesh(i);

        if (mesh && (_pScene->GetViewModelFlag() & ObjectService::MODEL || _pScene->GetViewModelFlag() & ObjectService::WIREFRAME))
        {
            geode->setName("WireFrame");
            geode->addDrawable(geometry);	

            // vertices, texcoords, normals
            int        numOfVertices= mesh->mesh_buffer->n / _vertexSize;

            //    SVertice*  vData        = _meshBufferList;
            Vec3Array* vertices     = new Vec3Array();
            Vec2Array* texcoords    = new Vec2Array();
            Vec3Array* normals      = new Vec3Array();

            vertices->reserve(numOfVertices);
            for (int v = 0; v < numOfVertices; ++v)
            {
                // vertex buffer
                vertices->push_back(Vec3(_meshBufferList[i][v].position[0], _meshBufferList[i][v].position[1], _meshBufferList[i][v].position[2]));
                // texcoords
                texcoords->push_back(Vec2(fabs(_meshBufferList[i][v].texture_vertex[0]), fabs(_meshBufferList[i][v].texture_vertex[1])));
                // normals
                normals->push_back(Vec3(_meshBufferList[i][v].normal[0], _meshBufferList[i][v].normal[1], _meshBufferList[i][v].normal[2]));
            }

            geometry->setVertexArray(vertices);
            geometry->setTexCoordArray(0, texcoords);
            geometry->setNormalArray(normals);

            // faces
            //TODO: mozna to zrobic jeszcze lepiej. poprawic troszczeke
            DrawElementsUInt* face;
            for (int f = 0; f < mesh->mesh_faces->face_count; ++f)
            {
                face = new DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
                face->push_back(mesh->mesh_faces->faces[f].index[0]);
                face->push_back(mesh->mesh_faces->faces[f].index[1]);
                face->push_back(mesh->mesh_faces->faces[f].index[2]);
                geometry->addPrimitiveSet(face);
            }
        }

        osg::StateSet* stateset = new osg::StateSet;
        osg::PolygonOffset* polyoffset = new osg::PolygonOffset;
        polyoffset->setFactor(-1.0f);
        polyoffset->setUnits(-1.0f);
        osg::PolygonMode* polymode = new osg::PolygonMode;
        polymode->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);
        stateset->setAttributeAndModes(polyoffset,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
        stateset->setAttributeAndModes(polymode,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);

#if 1
        osg::Material* material = new osg::Material;
        stateset->setAttributeAndModes(material,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);

        stateset->setMode(GL_LIGHTING,osg::StateAttribute::OVERRIDE|osg::StateAttribute::OFF);
#else
        // version which sets the color of the wireframe.
        osg::Material* material = new osg::Material;
        material->setColorMode(osg::Material::OFF); // switch glColor usage off
        // turn all lighting off 
        material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0,0.0f,0.0f,1.0f));
        material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0,0.0f,0.0f,1.0f));
        material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0,0.0f,0.0f,1.0f));
        // except emission... in which we set the color we desire
        material->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0,1.0f,0.0f,1.0f));
        stateset->setAttributeAndModes(material,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
        stateset->setMode(GL_LIGHTING,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
#endif

        geode->setStateSet(stateset);
    }
}

//--------------------------------------------------------------------------------------------------
// applies material 
bool Model::ApplyMaterial(unsigned int mesh_id, unsigned int id,  bool isVisible)
{
    // TODO:
    // at the moment - take diffuse texture and load it
    // we should load proper shader... etc

    if(_materialId < 0)
        return false;

    vector<SMaterial>* materialList = ((CCustomGroup*)_root.get())->getMaterialList();

    if (materialList && materialList->size() > id)
    {
        texmap::iterator i;
        if ((i = (*materialList)[id].texture_list.find(ETextureType::DIFFUSE_MAP)) != (*materialList)[id].texture_list.end())
        {
            Texture2D* tex = new Texture2D();

            wstring path = ((CCustomGroup*)_root.get())->getPath() + L"/" + i->second;
            tex->setDataVariance(osg::Object::DYNAMIC); 
            osg::Image* klnFace = osgDB::readImageFile(string(path.begin(), path.end()));
            if (!klnFace) return false;
            tex->setImage(klnFace);

            StateSet* stateOne = new StateSet();


            if(isVisible)
                stateOne->setTextureAttributeAndModes(0, tex, osg::StateAttribute::OVERRIDE|StateAttribute::ON);
            else
                stateOne->setTextureAttributeAndModes(0, tex, osg::StateAttribute::OVERRIDE|StateAttribute::OFF);

            _meshes[mesh_id]->setStateSet(stateOne);
        }
        else
            return false;
    }
    return true;
}


//--------------------------------------------------------------------------------------------------
// removes geodes
void Model::RemoveGeodes()
{
    _meshes.clear();
    if(!_root)
        return;
    int index = 0;
    int childNum = _root->getNumChildren();
    for (int i = childNum - 1; i >= 0; --i)
    {	
        index++;
        Geode* geode = dynamic_cast<Geode*>(_root->getChild(i));
        if (geode)
            _root->removeChild(geode);   
    }	
}

//--------------------------------------------------------------------------------------------------
bool Model::LoadModel()
{
    ref_ptr<Geode>	geode = new Geode();

    // for every mesh
    for (unsigned int i = 0; i < (((CCustomGroup*)_root.get())->getNumOfMeshes()); ++i)
    {
        SMesh* mesh = ((CCustomGroup*)_root.get())->getMesh(i);

        if (mesh && (_pScene->GetViewModelFlag() & ObjectService::MODEL || _pScene->GetViewModelFlag() & ObjectService::WIREFRAME))
        {
            std::string name = "mesh";
            std::string zmienna;
            itoa(i,zmienna, 10);
            name.append(zmienna);

            geode->setName(name);
            geode->addDrawable(mesh);	


            // vertices, texcoords, normals
            int numOfVertices= mesh->mesh_buffer->n / _vertexSize;

            SVertice*  vData        = _meshBufferList[i];
            Vec3Array* vertices     = new Vec3Array();
            Vec2Array* texcoords    = new Vec2Array();
            Vec3Array* normals      = new Vec3Array();


            vertices->reserve(numOfVertices);
            for (int v = 0; v < numOfVertices; v++) 
            {
                // vertex buffer
                vertices->push_back(Vec3(vData[v].position[0], vData[v].position[1], vData[v].position[2]));
                // texcoords
                texcoords->push_back(Vec2(fabs(vData[v].texture_vertex[0]), fabs(vData[v].texture_vertex[1])));
                // normals
                normals->push_back(Vec3(vData[v].normal[0], vData[v].normal[1], vData[v].normal[2]));
            }

            mesh->setVertexArray(vertices);
            mesh->setTexCoordArray(0, texcoords);
            mesh->setNormalArray(normals);


            // faces
            for (int f = 0; f < mesh->mesh_faces->face_count; f++)
            {
                DrawElementsUInt* face = new DrawElementsUInt(_pScene->GetPrimitiveModeFlag(), 0);
                //  face->reserve(3);
                face->push_back(mesh->mesh_faces->faces[f].index[0]);
                face->push_back(mesh->mesh_faces->faces[f].index[1]);
                face->push_back(mesh->mesh_faces->faces[f].index[2]);
                
                mesh->addPrimitiveSet(face);
            }
        }

        mesh->dirtyBound();
        mesh->dirtyDisplayList();
    }

        _root->addChild(geode);
        _meshes.push_back(geode);
        _isModelLoaded = true;

    return true;
}

//--------------------------------------------------------------------------------------------------
// update mesh
bool Model::UpdateMesh()
{
    // for every mesh
    for (unsigned int i = 0; i < (((CCustomGroup*)_root.get())->getNumOfMeshes()); i++)
    {
        SMesh* mesh = ((CCustomGroup*)_root.get())->getMesh(i);

        if (mesh && (_pScene->GetViewModelFlag() & ObjectService::MODEL || _pScene->GetViewModelFlag() & ObjectService::WIREFRAME))
        {
            // vertices, texcoords, normals
            int numOfVertices= mesh->mesh_buffer->n / _vertexSize;

            SVertice*  vData        = _meshBufferList[i];
            Vec3Array* vertices     = new Vec3Array();
            Vec2Array* texcoords    = new Vec2Array();
            Vec3Array* normals      = new Vec3Array();


            vertices->reserve(numOfVertices);
            for (int v = 0; v < numOfVertices; v++) 
            {
                // vertex buffer
                vertices->push_back(Vec3(vData[v].position[0], vData[v].position[1], vData[v].position[2]));
                // texcoords
                texcoords->push_back(Vec2(fabs(vData[v].texture_vertex[0]), fabs(vData[v].texture_vertex[1])));
                // normals
                normals->push_back(Vec3(vData[v].normal[0], vData[v].normal[1], vData[v].normal[2]));
            }

            mesh->setVertexArray(vertices);
            mesh->setTexCoordArray(0, texcoords);
            mesh->setNormalArray(normals);


            // faces
            for (int f = 0; f < mesh->mesh_faces->face_count; f++)
            {
                DrawElementsUInt* face = new DrawElementsUInt(_pScene->GetPrimitiveModeFlag(), 0);
                //  face->reserve(3);
                face->push_back(mesh->mesh_faces->faces[f].index[0]);
                face->push_back(mesh->mesh_faces->faces[f].index[1]);
                face->push_back(mesh->mesh_faces->faces[f].index[2]);
                    
                mesh->setPrimitiveSet(f,face);
            }
        }

        mesh->dirtyBound();
        mesh->dirtyDisplayList();
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool Model::LoadShaderSource(osg::Shader* obj, const std::string& file)
{
    std::string fqFileName = osgDB::findDataFile(file);
    if (!fqFileName.length())
        return false;

    if (obj->loadShaderSourceFromFile( fqFileName.c_str()))
        return true;
    else
        return false;
}

//--------------------------------------------------------------------------------------------------
bool Model::AddShaders(const std::string& vertex_address, const std::string& pixel_address)
{
    for (std::vector<osg::ref_ptr<osg::Geode> >::iterator i = _meshes.begin(); i != _meshes.end(); ++i)
    {
        if ((*i).valid())
        {
            StateSet* state   = (*i)->getOrCreateStateSet();

            Program* program  = new Program;
            Shader*  vertex   = new Shader(osg::Shader::VERTEX);
            Shader*  fragment = new Shader(osg::Shader::FRAGMENT);

            program->addShader(vertex);
            program->addShader(fragment);

            LoadShaderSource(vertex,   vertex_address);
            LoadShaderSource(fragment, pixel_address);

            state->setAttributeAndModes(program, StateAttribute::ON);	
        }
    }

    return false;
}

//--------------------------------------------------------------------------------------------------
bool Model::IsValid()
{
    return _properlyLoaded; 
}

//--------------------------------------------------------------------------------------------------
osg::ref_ptr<osg::Group> Model::GetRoot()
{
    return _root; 
}

//--------------------------------------------------------------------------------------------------
void Model::ResetWireFrame()
{
    if (_wireFrameGeode.valid())
        _root->removeChild(_wireFrameGeode.get());
}
