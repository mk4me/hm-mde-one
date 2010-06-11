#ifndef MODEL_WITH_SKELETON_H
#define MODEL_WITH_SKELETON_H


#include <osg/PositionAttitudeTransform>
#include <osg/Geode>
#include <vector>

#include "Model.h"
#include "Animation.h"
#include "../../osgDBPlugin/FAnimation.h"


#define MAX_AFFECTING_BONES 8


typedef osg::ref_ptr<osg::PositionAttitudeTransform> posatttransref;


//////////////////////////////////////////////////////////////////////////
// struct used for holding bone
struct STransformation
{
	// for hierarchy recalculation 
	osg::Vec3d	position;		// start point
	osg::Quat	attitude;		// this bone rotation

	// TODO:
	// na razie wystepuje spora redundancja danych, ale chce aby to najpierw
	// zadzialalo... poza tym w pewien sposob jest to optymalizacja...

	// for skinning purpose
	osg::Vec3d	point;			// bone position in space
	osg::Vec3d	bone;			// vector describing bone
	osg::Vec3d	translation;	// vector translation of bone during animation
	osg::Quat	rotation;		// rotation: orig. bone -> act bone
};


//////////////////////////////////////////////////////////////////////////
// model that has skeleton
class ModelWithSkeleton : public Model
{		
    ObjectService* _pScene;

	// animations
	std::map<std::string, Animation*>	_animations;

	// skeleton
	osg::ref_ptr<osg::Group>			_skeleton;

	// vector of pointers at bones
	std::vector<SkeletonNode*>*		_joints;

	// geode of skeleton
	osg::ref_ptr<osg::Geode>			_skeletonGeode;

	// vector holding initial bone transformations
	STransformation*					_initialBones;
	// vector holding actual bones - transformed!
	STransformation*					_actualBones;
	// number of bones
	unsigned int						_numOfBones;
	// additional counter
	unsigned int				        _counter;

    std::string                         _selectedGroupName;

	// temporary array
	osg::Vec3d							_tempVectors[MAX_AFFECTING_BONES][2];

	// last played animation
	std::string							_lastPlayedAnimation;

	// create arrays of bones
	void CreateArrayOfJoints(SkeletonNode* bone);
	// draws one bone
	void DrawBone(osg::PositionAttitudeTransform* bone, const osg::Vec3d* parentPos, const osg::Quat* parentRot, osg::Geode* geode);
	// update bone
	void UpdateBone(osg::PositionAttitudeTransform* bone);

    void getTransformToGroup( osg::PositionAttitudeTransform* bone, const osg::Vec3d* parentPos, const osg::Quat* parentRot, osg::Group* group);
	// recalculates changes of skeleton
	void RecalculateChanges();

    osg::ref_ptr<osg::Geometry> drawLine(const osg::Vec3d* startPos, const osg::Vec3d* endPos);

    void drawGroup(osg::PositionAttitudeTransform* bone, const osg::Vec3d* parentPos, const osg::Quat* parentRot, osg::Geode* geode);


public:

	// get name of last got animation
	std::string&						getLastName()		{ return _lastPlayedAnimation; }

	// get animation
	Animation*							GetAnimation(std::string& name);
	// get animations
	std::map<std::string, Animation*>*	getAnimations()		{ return &_animations; }

	// returns pointer at specified bone
	osg::PositionAttitudeTransform*		getBone(unsigned int id) { return (*_joints)[id]; }

	// returns pointer at skeleton
	osg::PositionAttitudeTransform*		getSkeleton() { return (osg::PositionAttitudeTransform*)(_skeleton.valid() ? _skeleton.get() : NULL);  }

	// update skeleton when called
	void UpdateSkeleton();

	// updates skeleton's MESH! - only shows skeleton!!! 
	// you have to update skeleton first by yourself!
	// i assume this is debug/information function

    void SelectGroup(std::string nameGroup);
    void findSelectedGroup(osg::Group* group, std::string nameGroup);
    void drawSelectedGroup(osg::Group* group);


    // Draw Bones
	void UpdateSkeletonMesh();

	// draws mesh
	virtual bool UpdateMesh();

	ModelWithSkeleton(const std::string& fileName, bool visible = true);
    ModelWithSkeleton(bool visible, std::vector<SkeletonNode*>* joints, STransformation* initialBones, std::string _lastPlayedAnimation, STransformation* actualBones, SSkeleton* skeleton, SFAnimation* animation, SVertice* meshBuffer);
	virtual ~ModelWithSkeleton();
};

#endif
