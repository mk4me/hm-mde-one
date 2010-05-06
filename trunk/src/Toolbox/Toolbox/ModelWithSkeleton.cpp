#include "ModelWithSkeleton.h"

#include <osg/Geometry>
#include <algorithm>
#include <math.h>

#include "ServiceManager.h"
#include "ObjectService.h"

#include "../OsgExtensions/CustomGroup.h"
#include "../OsgExtensions/SkeletonNode.h"

// helper - this name is quite long...
#define pPat osg::PositionAttitudeTransform*

#define POSITION 0
#define NORMALS  1

using namespace osg;
using namespace std;


//////////////////////////////////////////////////////////////////////////
// recalculates changes of skeleton
void CModelWithSkeleton::recalculateChanges()
{
	for (unsigned int b = 0; b < _numOfBones; ++b)
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


//////////////////////////////////////////////////////////////////////////
// update skeleton
void CModelWithSkeleton::updateSkeleton()
{
	pPat skeleton = dynamic_cast<CSkeletonNode*>(_skeleton.get());
	if (skeleton)
	{
		_counter = 0;

		for (unsigned int b = 0; b < skeleton->getNumChildren(); ++b)
			if (dynamic_cast<CSkeletonNode*>(skeleton->getChild(b)) )
				updateBone((pPat)skeleton->getChild(b));
	}
}


//////////////////////////////////////////////////////////////////////////
// creates arrays of pointers at bones sorted by id
void CModelWithSkeleton::updateBone(osg::PositionAttitudeTransform* bone)
{
	unsigned int pId = ((CSkeletonNode*)bone->getParent(0))->getId();

	osg::Vec3d ppos = pId ? _actualBones[pId - 1].position : ((pPat)_skeleton.get())->getPosition();
	osg::Quat  prot = pId ? _actualBones[pId - 1].attitude : ((pPat)_skeleton.get())->getAttitude();


	osg::Vec3d bpos = prot * bone->getPosition() + ppos;
	osg::Quat  brot = bone->getAttitude() * prot;

	STransformation tr;
	tr.attitude	= brot;
	tr.position	= bpos;

	tr.bone		= bpos - ppos;
	tr.point	= ppos;

	_actualBones[_counter] = tr;
	++_counter;

	for (unsigned int b = 0; b < bone->getNumChildren(); ++b)
		if (dynamic_cast<CSkeletonNode*>(bone->getChild(b)))
			updateBone((pPat)bone->getChild(b));
}


//////////////////////////////////////////////////////////////////////////
// update mesh
bool CModelWithSkeleton::updateMesh()
{
	// TODO:

	// musze przesunac to z moich wczytanych z modelem struktur
	// do OSG

//     if(!_root)
//         return false;

    	removeGeodes();
		SMesh* mesh = ((CCustomGroup*)_root.get())->getMesh();
		SSkin* skin = NULL;

        if(_pScene->getViewModelFlag() & MODEL_FLAG::BONE)
            updateSkeletonMesh();

		if (mesh && (skin = mesh->modifier->skin))
		{
			// updates skeleton
			updateSkeleton();
			// recalculates changes of skeleton
			recalculateChanges();
	
			// update all vertices
			SVertice* vData = (SVertice*)mesh->mesh_buffer->buffer;
	
			// update vertices using skin
			for (int i = 0; i < skin->n; ++i)
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
				for (int b = 0; b < vertice->n; ++b)
				{
					_tempVectors[b][POSITION] =	_actualBones[vertice->bones[b].boneID].rotation 
											  * (actPos - _actualBones[vertice->bones[b].boneID].point									
											  + _actualBones[vertice->bones[b].boneID].translation)									
											  + _actualBones[vertice->bones[b].boneID].point;
	
					_tempVectors[b][NORMALS]  = _actualBones[vertice->bones[b].boneID].rotation * normal;
				}
	
				// get output point
				Vec3d change, nchange;
				for (int b = 0; b < vertice->n; ++b)
				{
					change  += _tempVectors[b][POSITION] * vertice->bones[b].weight;
					nchange += _tempVectors[b][NORMALS]  * vertice->bones[b].weight;
				}
	
				// update of vertex
				_meshBuffer[vertice->vert_id].position[0] = change.x();
				_meshBuffer[vertice->vert_id].position[1] = change.y();
				_meshBuffer[vertice->vert_id].position[2] = change.z();
	
				// update of normal 
				_meshBuffer[vertice->vert_id].normal[0] = nchange.x();
				_meshBuffer[vertice->vert_id].normal[1] = nchange.y();
				_meshBuffer[vertice->vert_id].normal[2] = nchange.z();
			}
	
			// create new mesh
			CModel::updateMesh();
	
			return true;
		}
	

	return false;
}


//////////////////////////////////////////////////////////////////////////
// creates array of pointers at bones sorted by id
void CModelWithSkeleton::createArrayOfJoints(CSkeletonNode* bone)
{
	_joints->push_back(bone);

	for (unsigned int b = 0; b < bone->getNumChildren(); ++b)
		if (dynamic_cast<CSkeletonNode*>(bone->getChild(b)))
			createArrayOfJoints((CSkeletonNode*)bone->getChild(b));
}

//////////////////////////////////////////////////////////////////////////
// d - tor
CModelWithSkeleton::~CModelWithSkeleton()
{
	if (_joints)
		delete _joints;
	_joints = NULL;

	if (_initialBones)
		delete [] _initialBones;
	_initialBones = NULL;

	if (_actualBones)
		delete [] _actualBones;
	_actualBones = NULL;
}


//////////////////////////////////////////////////////////////////////////
// get animation
CAnimation*	CModelWithSkeleton::getAnimation(std::string& name)
{
	map<std::string, CAnimation*>::iterator i = _animations.find(name);

	if (i != _animations.end())
	{
		_lastPlayedAnimation = name;
		return i->second;
	}
	else
		return NULL;
}


//////////////////////////////////////////////////////////////////////////
// c - tor
CModelWithSkeleton::CModelWithSkeleton(const std::string &fileName, bool visible)
:	CModel(fileName, visible), _joints(NULL), _initialBones(NULL), 
	_lastPlayedAnimation(""), _actualBones(NULL)
{
    _pScene = ServiceManager::GetInstance()->GetSystemServiceAs<ObjectService>(); 

	if (_root.valid())
	{
		// find skeleton
		int childNum = _root->getNumChildren();
		for (int i = 0; i < childNum; ++i)
		{		
			Group* group = dynamic_cast<CSkeletonNode*>(_root->getChild(i));
			if (group)
			{
				//////////////////////////////////////////////////////////////////////////
				// get skeleton and set pointer at him as member
				_skeleton	= _root->getChild(i)->asGroup();

				// create vector of pointers at bones
				_joints		= new vector<CSkeletonNode*>();
				createArrayOfJoints((CSkeletonNode*)_skeleton->getChild(0)->asGroup());

				_numOfBones	= _joints->size();// - 1;

				// get 'global' (? - not another transformations but entire propagated from root) 
				// transformations of bones
				_actualBones  = new STransformation [_numOfBones];
				updateSkeleton();

				// save initial values
				_initialBones = new STransformation [_numOfBones];
				memcpy(_initialBones, _actualBones, sizeof(STransformation) * _numOfBones);

				//////////////////////////////////////////////////////////////////////////
				// handle animations

				// extract number of animations				
				unsigned int	numOfAnims = 0;
				vector<string>	names;
				for (vector<CSkeletonNode*>::iterator i = _joints->begin(); i != _joints->end(); ++i)
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
					CAnimation* animation = new CAnimation(_joints, i);
					_animations.insert(make_pair(names[i], animation));	
				}

				// TODO:
				// AFAIK - skeleton may/should be only one...
				break;
			}
		}	
	}
}

CModelWithSkeleton::CModelWithSkeleton(
    bool visible,
    std::vector<CSkeletonNode*>* joints,
    STransformation* initialBones,
    std::string _lastPlayedAnimation,
    STransformation* actualBones,
    SSkeleton* skeleton,
    SFAnimation* animation,
    SVertice* meshBuffer)

    : CModel(meshBuffer, visible)
    , _joints(joints)
    , _initialBones(initialBones)
    , _lastPlayedAnimation(_lastPlayedAnimation)
    , _actualBones(actualBones)
{

    ref_ptr<CCustomGroup> root = new CCustomGroup();

    // meshBuffer
    SFMesh* fmesh = new SFMesh();
    root->setFMesh(fmesh);

    // kosci

    root->setSkeleton(skeleton);

    root->setAnimations(animation);

    _root = root;
}


//////////////////////////////////////////////////////////////////////////
// draws bone
void CModelWithSkeleton::drawBone(pPat bone, const osg::Vec3d* parentPos, const osg::Quat* parentRot, osg::Geode* geode)
{
	osg::Vec3d bpos = (*parentRot) * bone->getPosition() + *parentPos;
	osg::Quat  brot = bone->getAttitude() * (*parentRot);

	// recursively draw other bones
	for (unsigned int b = 0; b < bone->getNumChildren(); ++b)
		if (dynamic_cast<CSkeletonNode*>(bone->getChild(b)))
	        drawBone((pPat)bone->getChild(b), &bpos, &brot, geode);


    // TODO: do rysowania ko�ci najlepiej uzyc macierzy, narazie szybki szpil.

	// draw actual bone
	osg::ref_ptr<osg::Geometry>  geometry = new osg::Geometry();

	// vertices
	osg::Vec3Array* vertices = new osg::Vec3Array();
	vertices->push_back(*parentPos);
	vertices->push_back(bpos);


	// indices
	osg::DrawElementsUInt* line = new osg::DrawElementsUInt(osg::PrimitiveSet::LINES, 0);
	line->push_back(0);
	line->push_back(1);

	// set geometry data
    geometry->setVertexArray(vertices);
	geometry->addPrimitiveSet(line);

    // set colors
    // sprawdzic jak wp�ywa na kolor textury.
    osg::Vec4Array* colors = new osg::Vec4Array;
    if(_selectedGroupName == bone->getName())
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

	// add to geode
	geode->addDrawable(geometry);

    //  Bone jak w 3dStudio Max - wymaga poprawy jeszcz i optyalizacji.  
    // TODO:  kosc powinna sie sk�adac z 4xtrujkat + 4xlinia.
/*
    	osg::Vec3d bpos2 = (*parentRot) * ((bone->getPosition()*4)/5) + *parentPos;
        osg::Vec3d bpos3 = (*parentRot) * ((bone->getPosition()*4)/5) + *parentPos;  
    
    bpos2._v[1] += 0.03f;
        geode->addDrawable(drawLine(parentPos, &bpos2));
        geode->addDrawable(drawLine(&bpos2, &bpos));
    
    bpos2._v[1] -= 0.06f;
        geode->addDrawable(drawLine(parentPos, &bpos2));
        geode->addDrawable(drawLine(&bpos2, &bpos));
    
    bpos2._v[1] += 0.03f;
    
    bpos3._v[0] += 0.03f;
        geode->addDrawable(drawLine(parentPos, &bpos3));
        geode->addDrawable(drawLine(&bpos3, &bpos));
    
    bpos3._v[0] -= 0.06f;
        geode->addDrawable(drawLine(parentPos, &bpos3));
        geode->addDrawable(drawLine(&bpos3, &bpos));
    
    bpos3._v[0] += 0.03f;
 
    bpos2._v[1] += 0.03f;
    bpos3._v[0] += 0.03f;
        geode->addDrawable(drawLine(&bpos3, &bpos2));
    
    bpos3._v[0] -= 0.06f;
        geode->addDrawable(drawLine(&bpos3, &bpos2));
    
    bpos2._v[1] -= 0.06f;
        geode->addDrawable(drawLine(&bpos3, &bpos2));
    
    bpos3._v[0] += 0.06f;
        geode->addDrawable(drawLine(&bpos3, &bpos2));*/
}


//////////////////////////////////////////////////////////////////////////
// draws skeleton
void CModelWithSkeleton::updateSkeletonMesh()
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
		if (dynamic_cast<CSkeletonNode*>(root_bone->getChild(b)))
			drawBone((pPat)root_bone->getChild(b), &root_bone->getPosition(), &root_bone->getAttitude(), _skeletonGeode);

	_root->addChild(_skeletonGeode);
}

void CModelWithSkeleton::selectGroup( std::string nameGroup)
{
    _selectedGroupName = nameGroup;
}


/*
//TODO:  do optymalizacji - za duzo posrednich wywo�a� !!!! R.Z
//////////////////////////////////////////////////////////////////////////
// select groups
void CModelWithSkeleton::selectGroup( std::string nameGroup)
{
    osg::Group *group = _root->asGroup();
    _selectedGroupName = NULL;

    if(group)
        findSelectedGroup(group, nameGroup);
}

//////////////////////////////////////////////////////////////////////////
// find group
void CModelWithSkeleton::findSelectedGroup( osg::Group* group, std::string nameGroup)
{
    if(group->getName() == nameGroup)
    {
        _selectedGroupName = group;
        return;
    }

    int childCount = group->getNumChildren(); 
    for (int i=0; i<childCount; ++i)
    {
        osg::Node *child = group->getChild(i); 
        if(child->asGroup() != 0)
            findSelectedGroup(child->asGroup(), nameGroup);
    }
}

osg::ref_ptr<osg::Geometry> CModelWithSkeleton::drawLine(const osg::Vec3d* startPos, const osg::Vec3d* endPos)
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





// Metody teraz nie u�ywane, mo�e si� przydadza potem

// medoty ssu��ce do rysowania ko�ci - tylko pojedynczych grup, bez koniecznosci rysowania ca�ego szkieletu
//////////////////////////////////////////////////////////////////////////
// draw selected group
void CModelWithSkeleton::drawSelectedGroup( osg::Group* group )
{
    pPat root_group = (pPat)(group);
    pPat root_bone = (pPat)(_skeleton.get());

    for(unsigned int i = 0; i < root_bone->getNumChildren(); i++)
        if(dynamic_cast<pPat>(root_bone->getChild(i)))
            getTransformToGroup((pPat)root_bone->getChild(i), &root_bone->getPosition(), &root_bone->getAttitude(), group);
}


void CModelWithSkeleton::getTransformToGroup( osg::PositionAttitudeTransform* bone, const osg::Vec3d* parentPos, const osg::Quat* parentRot, osg::Group* group )
{
    if(bone->getName() == group->getName())
    {   
        osg::Geode* geode = new osg::Geode();
        geode->setName("test_DrawSelectedGroup");
        drawGroup(bone, parentPos, parentRot, geode);

        _root->addChild(geode);
    }

    osg::Vec3d bpos = (*parentRot) * bone->getPosition() + *parentPos;
    osg::Quat  brot = bone->getAttitude() * (*parentRot);

    // recursively draw other bones
    for (unsigned int b = 0; b < bone->getNumChildren(); ++b)
        if (dynamic_cast<pPat>(bone->getChild(b)))
            getTransformToGroup((pPat)bone->getChild(b), &bpos, &brot, group);
}

//////////////////////////////////////////////////////////////////////////
// proawdziwa metoda rysuj�ca - tworz�ca wierzcho�ki itp.
void CModelWithSkeleton::drawGroup( osg::PositionAttitudeTransform* bone, const osg::Vec3d* parentPos, const osg::Quat* parentRot, osg::Geode* geode )
{
    osg::Vec3d bpos = (*parentRot) * bone->getPosition() + *parentPos;
    osg::Quat  brot = bone->getAttitude() * (*parentRot);

    // recursively draw other bones
    for (unsigned int b = 0; b < bone->getNumChildren(); ++b)
        if (dynamic_cast<pPat>(bone->getChild(b)))
            drawGroup((pPat)bone->getChild(b), &bpos, &brot, geode);


    // tworzymy wierzcholki - do narysowania obramowania grupy ( tymczasowo kosc na inny kolor)
    // draw actual linie
    osg::ref_ptr<osg::Geometry>  geometry = new osg::Geometry();

    // vertices
    osg::Vec3Array* vertices = new osg::Vec3Array();
    vertices->push_back(*parentPos);
    vertices->push_back(bpos);


    // indices
    osg::DrawElementsUInt* line = new osg::DrawElementsUInt(osg::PrimitiveSet::LINES, 0);
    line->push_back(0);
    line->push_back(1);

    // set geometry data
    geometry->setVertexArray(vertices);
    geometry->addPrimitiveSet(line);

    // set colors
    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
    colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));

    osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType,4,4> *colorIndexArray;
    colorIndexArray = new osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType,4,4>;
    colorIndexArray->push_back(0);
    colorIndexArray->push_back(1);

    geometry->setColorArray(colors);
    geometry->setColorIndices(colorIndexArray);
    geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    // add to geode
    geode->addDrawable(geometry);

}*/
