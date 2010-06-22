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

//--------------------------------------------------------------------------------------------------
ModelWithSkeleton::ModelWithSkeleton(const std::string &fileName, bool visible):
  Model(fileName, visible)
, _joints(NULL)
, _initialBones(NULL) 
, _lastPlayedAnimation("")
, _actualBones(NULL)
{
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

                _numOfBones	= _joints->size();// - 1;

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
                unsigned int	numOfAnims = 0;
                vector<string>	names;
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
ModelWithSkeleton::ModelWithSkeleton(bool visible, std::vector<SkeletonNode*>* joints, STransformation* initialBones, 
                                     std::string _lastPlayedAnimation, STransformation* actualBones, SSkeleton* skeleton, 
                                     SFAnimation* animation, SVertice* meshBuffer):
  Model(meshBuffer, visible)
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

//--------------------------------------------------------------------------------------------------
ModelWithSkeleton::~ModelWithSkeleton()
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

//--------------------------------------------------------------------------------------------------
// recalculates changes of skeleton
void ModelWithSkeleton::RecalculateChanges()
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

//--------------------------------------------------------------------------------------------------
// update skeleton
void ModelWithSkeleton::UpdateSkeleton()
{
	pPat skeleton = dynamic_cast<SkeletonNode*>(_skeleton.get());
	if (skeleton)
	{
		_counter = 0;

		for (unsigned int b = 0; b < skeleton->getNumChildren(); ++b)
			if (dynamic_cast<SkeletonNode*>(skeleton->getChild(b)) )
				UpdateBone((pPat)skeleton->getChild(b));
	}
}


//--------------------------------------------------------------------------------------------------
// creates arrays of pointers at bones sorted by id
void ModelWithSkeleton::UpdateBone(osg::PositionAttitudeTransform* bone)
{
	unsigned int pId = ((SkeletonNode*)bone->getParent(0))->GetId();

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
		if (dynamic_cast<SkeletonNode*>(bone->getChild(b)))
			UpdateBone((pPat)bone->getChild(b));
}

//--------------------------------------------------------------------------------------------------
// update mesh
bool ModelWithSkeleton::UpdateMesh()
{
    // TODO: musze przesunac to z moich wczytanych z modelem struktur do OSG

    //if(!_root)
    //    return false;

    RemoveGeodes();
    SMesh* mesh = ((CCustomGroup*)_root.get())->getMesh();
    SSkin* skin = NULL;

    if(_pScene->GetViewModelFlag() & ObjectService::BONE)
        UpdateSkeletonMesh();

    if (mesh && (skin = mesh->modifier->skin))
    {
        // updates skeleton
        UpdateSkeleton();
        // recalculates changes of skeleton
        RecalculateChanges();

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
        Model::UpdateMesh();

        return true;
    }
    return false;
}

//--------------------------------------------------------------------------------------------------
// creates array of pointers at bones sorted by id
void ModelWithSkeleton::CreateArrayOfJoints(SkeletonNode* bone)
{
	_joints->push_back(bone);

	for (unsigned int b = 0; b < bone->getNumChildren(); ++b)
		if (dynamic_cast<SkeletonNode*>(bone->getChild(b)))
			CreateArrayOfJoints((SkeletonNode*)bone->getChild(b));
}

//--------------------------------------------------------------------------------------------------
// get animation
Animation*	ModelWithSkeleton::GetAnimation(std::string& name)
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
void ModelWithSkeleton::DrawBone(pPat bone, const osg::Vec3d* parentPos, const osg::Quat* parentRot, osg::Geode* geode)
{
	osg::Vec3d bpos = (*parentRot) * bone->getPosition() + *parentPos;
	osg::Quat  brot = bone->getAttitude() * (*parentRot);

	// recursively draw other bones
	for (unsigned int b = 0; b < bone->getNumChildren(); ++b)
		if (dynamic_cast<SkeletonNode*>(bone->getChild(b)))
			DrawBone((pPat)bone->getChild(b), &bpos, &brot, geode);
		

    // TODO  zmienic nazwe bone na joint, bo to strasznie myli, a naprawde to zmeiniemy pozycje jointów
    // problem polega na tym, ze w momencie gdy joint ma iecej niz 1 dziecko - przesyla im wszystkim takiego samego quata (rotacje)
    // w skutek czego mamy kilka kosciu u³ozonych w ten sam sposób.

    // JOINTY to NIE KOŒCI - mozna je traktowaæ jako spoiwa miêdzy nimi. ale nie jako same koœci.

// 	osg::Vec3d temp_1 = boneNodeChild - boneNodeParent;
// 	osg::Vec3d temp_2 = boneNodeParent - boneNodeChild;
// 
// 	result._v[0] = (temp_1._v[1]*temp_2[2] - temp_1._v[2]*temp_2[1]);
// 	result._v[1] = (temp_1._v[0]*temp_2[2] - temp_1._v[2]*temp_2[0]);
// 	result._v[2] = (temp_1._v[0]*temp_2[1] - temp_1._v[1]*temp_2[0]);
// 
// 	// 	double norm = result.length();
// 	// 	if(norm > 0)
// 	// 	{
// 	// 		double inv = 1.0f/norm;
// 	// 		result._v[0] *= inv;
// 	// 		result._v[1] *= inv;
// 	// 		result._v[2] *= inv;
// 	// 	}
// 
// 	boneNodeLeftArm = osg::Vec3(distanceHightToArm, -distanceWidthToArm, 0.f);
// 
// 	osg::Quat q(PI_2, boneNodeChild);
// 	boneNodeRightArm = (q * punkt);
// 
// 
// 	osg::Quat q2(PI, boneNodeChild);
// 	boneNodeFrontArm = (q2 * punkt);
// 
// 
// 	osg::Quat q3((PI+PI_2), boneNodeChild);
// 	boneNodeBackArm = (q3 * punkt);
// 
// 
// 	// 	osg::Matrix3 m(1.0,0.0,0.0,
// 	// 				   0.0,1.0,0.0,
// 	// 				   0.0,0.0,1.0);


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


    geode->addDrawable(DrawTriangle(&boneNodeParent, &boneNodeLeftArm, &boneNodeFrontArm, &boneNodeRightArm, &boneNodeBackArm, &boneNodeLeftArm));
	geode->addDrawable(DrawTriangle(&boneNodeChild, &boneNodeLeftArm, &boneNodeFrontArm, &boneNodeRightArm, &boneNodeBackArm, &boneNodeLeftArm));

//     geode->addDrawable(DrawLine(&boneNodeParent, &boneNodeLeftArm));
//     geode->addDrawable(DrawLine(&boneNodeLeftArm, &boneNodeChild));
//     geode->addDrawable(DrawLine(&boneNodeRightArm, &boneNodeChild));
//     geode->addDrawable(DrawLine(&boneNodeRightArm, &boneNodeParent));
// 
//     geode->addDrawable(DrawLine(&boneNodeParent, &boneNodeFrontArm));
//     geode->addDrawable(DrawLine(&boneNodeFrontArm, &boneNodeChild));
//     geode->addDrawable(DrawLine(&boneNodeBackArm, &boneNodeChild));
//     geode->addDrawable(DrawLine(&boneNodeBackArm, &boneNodeParent));

  //  geode->addDrawable(DrawLine(parentPos, &bpos));
}

//--------------------------------------------------------------------------------------------------
void ModelWithSkeleton::UpdateSkeletonMesh()
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
osg::ref_ptr<osg::Geometry> ModelWithSkeleton::DrawLine(const osg::Vec3d* startPos, const osg::Vec3d* endPos)
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
osg::ref_ptr<osg::Geometry> ModelWithSkeleton::DrawTriangle(const osg::Vec3d* startPos, const osg::Vec3d* endPos, const osg::Vec3d* vertexPos, const osg::Vec3d* startPos2, const osg::Vec3d* endPos2, const osg::Vec3d* vertexPos2)
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
	colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 0.0f));
	colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 0.0f));

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
void ModelWithSkeleton::SelectGroup( std::string nameGroup)
{
    _selectedGroupName = nameGroup;
}

/*
//TODO: do optymalizacji - za duzo posrednich wywo³añ !!!! 
//--------------------------------------------------------------------------------------------------
void ModelWithSkeleton::SelectGroup( std::string nameGroup)
{
    osg::Group *group = _root->asGroup();
    _selectedGroupName = NULL;

    if(group)
        findSelectedGroup(group, nameGroup);
}

//--------------------------------------------------------------------------------------------------
void ModelWithSkeleton::FindSelectedGroup( osg::Group* group, std::string nameGroup)
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

//--------------------------------------------------------------------------------------------------
osg::ref_ptr<osg::Geometry> ModelWithSkeleton::DrawLine(const osg::Vec3d* startPos, const osg::Vec3d* endPos)
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

// Metody teraz nie u¿ywane, mo¿e siê przydadza potem

// medoty ssu¿¹ce do rysowania koœci - tylko pojedynczych grup, bez koniecznosci rysowania ca³ego szkieletu
//--------------------------------------------------------------------------------------------------
// draw selected group
void ModelWithSkeleton::DrawSelectedGroup( osg::Group* group )
{
    pPat root_group = (pPat)(group);
    pPat root_bone = (pPat)(_skeleton.get());

    for(unsigned int i = 0; i < root_bone->getNumChildren(); i++)
        if(dynamic_cast<pPat>(root_bone->getChild(i)))
            getTransformToGroup((pPat)root_bone->getChild(i), &root_bone->getPosition(), &root_bone->getAttitude(), group);
}

//--------------------------------------------------------------------------------------------------
void CModelWithSkeleton::GetTransformToGroup( osg::PositionAttitudeTransform* bone, const osg::Vec3d* parentPos, const osg::Quat* parentRot, osg::Group* group )
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

//--------------------------------------------------------------------------------------------------
// proawdziwa metoda rysuj¹ca - tworz¹ca wierzcho³ki itp.
void CModelWithSkeleton::DrawGroup( osg::PositionAttitudeTransform* bone, const osg::Vec3d* parentPos, const osg::Quat* parentRot, osg::Geode* geode )
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

}
/**/
