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

    // TODO: do rysowania koœci najlepiej uzyc macierzy, narazie szybki szpil.
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
    // sprawdzic jak wp³ywa na kolor textury.
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

    //R.Zowal 16-06-2010
    /*
        Idea jest taka:
        1) tworzymy prost¹ prostopad³¹(wekstor) do aktualnej koœci - pos³u¿y to za prost¹ le¿¹c¹ na p³aszczyŸnie zbudowanej z tych dwóch wektorów
        2) tworzymy p³aszczyznê która jest prostopad³a do wczeœniej utworzonej - wektorem normalnym jest utworzony przez nas wektor (prostopad³y do koœci)
        3) tworzymy okr¹g którego œrodkiem jest koœæ a rysowany jest na p³aszczysnie prostopad³ej do koœci.
    */

//     vector V = C - A;
//     vector N = B - A;
//     float t = (V.x*N.x + V.y*N.y + V.z*N.z) / (N.x*N.x + N.y*N.y + N.z*N.z) ;
//     vector D = N * t;

//     osg::Vec3d bpos2 = (*parentRot) * ((bone->getPosition()*4)/5) + *parentPos;
// 
//     osg::Vec3d vecTemp;
// 
//     vecTemp._v[0] = bpos._v[0] - bpos2._v[0];
//     vecTemp._v[1] = bpos._v[1] - bpos2._v[1];
//     vecTemp._v[2] = bpos._v[2] - bpos2._v[2];
// 
// 
//     osg::Vec3d vecTemp2 = vecTemp;
//     vecTemp2._v[0] = -vecTemp2._v[0];
// 
//     osg::Vec3d punkt;
//     
//     punkt._v[0] = bpos2._v[0] + vecTemp2._v[0];
//     punkt._v[1] = bpos2._v[1] + vecTemp2._v[1];
//     punkt._v[2] = bpos2._v[2] + vecTemp2._v[2];
// 
//     geode->addDrawable(drawLine(&bpos2, &punkt));


    osg::Vec3d bpos2 = (*parentRot) * ((bone->getPosition()*4)/5) + *parentPos;
    osg::Vec3 result;

 //   [cx,cy,cz]  =  [  (ay*bz - az*by)  ,  (ax*bz - az*bx)  ,  (ax*by - ay*bx)  ]  // tworzenie wektora prostopad³ego.


    result._v[0] = (bpos._v[1]*bpos2[2] - bpos._v[2]*bpos2[1]);
    result._v[1] = (bpos._v[0]*bpos2[2] - bpos._v[2]*bpos2[0]);
    result._v[2] = (bpos._v[0]*bpos2[1] - bpos._v[1]*bpos2[0]);

    osg::Vec3d punkt;

    // niestety normalizacja w osg odadaje nam typ Vec3f - tylko i wy³¹cznie, a ca³a apliakcja uzywa Vec3d - daltego trzeba to zrobiæ rêcznie.
    double norm = result.length();
    if(norm > 0)
    {
        double inv = 1.0f/norm;
        result._v[0] *= inv;
        result._v[1] *= inv;
        result._v[2] *= inv;
    }

    punkt = bpos2 + result;

    geode->addDrawable(drawLine(&bpos, &punkt));
    geode->addDrawable(drawLine(&bpos2, &punkt));
    geode->addDrawable(drawLine(parentPos, &punkt));



    osg::Quat q(PI_2, bpos2);

    osg::Vec3d punkt2;
    punkt2 = (q * punkt);

    geode->addDrawable(drawLine(&bpos, &punkt2));
    geode->addDrawable(drawLine(parentPos, &punkt2));

    osg::Quat q2(PI, bpos2);

    osg::Vec3d punkt3;
    punkt3 = (q2 * punkt);

    geode->addDrawable(drawLine(&bpos, &punkt3));
    geode->addDrawable(drawLine(parentPos, &punkt3));


    osg::Quat q3((PI+PI_2), bpos2);
    osg::Vec3d punkt4;
    punkt4 = (q3 * punkt);

    geode->addDrawable(drawLine(&bpos, &punkt4));
    geode->addDrawable(drawLine(parentPos, &punkt4));





// 
//     osg::Vec3 boneVector;
//     boneVector = bpos - (*parentPos);
// 
// 
// 
// 
//     osg::Quat q(PI_2, boneVector);
// 
// 
//     osg::Vec3d punkt2;
//     punkt2 = q * punkt;
// 
//     geode->addDrawable(drawLine(&bpos, &punkt2));
//     geode->addDrawable(drawLine(parentPos, &punkt2));



//     float r = (bpos2 - punkt).length();
//     float phi = acos(punkt._v[2]/result.length());
//     float ro = atan2(punkt._v[1],punkt._v[0]);
// 
//     phi += PI;
//     ro += PI;
// 
//     osg::Vec3d punkt2;
//     punkt2._v[0] = (result.length()*sin(phi)*cos(ro)) + bpos2._v[0];
//     punkt2._v[1] = (result.length()*sin(phi)*sin(ro)) + bpos2._v[1];
//     punkt2._v[2] = (result.length()*cos(phi)) + bpos2._v[2];
// 
//     geode->addDrawable(drawLine(&bpos, &punkt2));
//     geode->addDrawable(drawLine(parentPos, &punkt2));




//    vecU * vecW = cos($) *|vecU||vecW|


//     // równianie p³aszczyzny     Ax+By+Cz+D=0
//     osg::Vec3 normalVector;
//     
//     //normalVector = prostopadly do bpos;
//     
//     osg::Plane plane_perpendicular;
// 
//     //plane_perpendicular = Plane stworzonego z normalVector i bpos;
// 
//     //  rysowanie okrêgu na plane_perpendicular - srodek okrêgu wyznacza przeciêcie bona z p³aszczyzna(plane_perpendicular)
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
osg::ref_ptr<osg::Geometry> ModelWithSkeleton::drawLine(const osg::Vec3d* startPos, const osg::Vec3d* endPos)
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
