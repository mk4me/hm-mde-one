#ifndef MODEL_H
#define MODEL_H


#include <osg/PositionAttitudeTransform>
#include <osg/Geode>
#include <vector>

#include "Model.h"
#include "Animation.h"
#include "../../osgDBPlugin/FAnimation.h"

// By³o w Model - zredukowac potem.
#include "../osgDBPlugin/ModelData.h"
#include <osg/Node>
#include <osg/Geode>
#include <osg/Shader>
#include <string>


#define MAX_AFFECTING_BONES 8


typedef osg::ref_ptr<osg::PositionAttitudeTransform> posatttransref;
typedef osg::ref_ptr<osg::Geode> geoderef;

class ObjectService;

//////////////////////////////////////////////////////////////////////////
// struct used for holding bone
struct STransformation
{
	// for hierarchy recalculation 
	osg::Vec3d	position;		// start point
	osg::Quat	attitude;		// this bone rotation
    bool hasChild;

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
class Model
{		

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
	virtual bool UpdateBones();

	Model(const std::string& fileName, bool visible = true);

    Model(
        bool visible, 
        std::vector<SkeletonNode*>* joints, 
        STransformation* initialBones, 
        std::string _lastPlayedAnimation, 
        STransformation* actualBones, 
        SSkeleton* skeleton, 
        SFAnimation* animation, 
        SVertice* meshBuffer);

	~Model();

    //Z Modelu
    void InicializeMesh(const std::string& fileName, bool visible);
    void InicializeMesh(SVertice* meshBuffer,  bool visible);
    // return root
    osg::ref_ptr<osg::Group> GetRoot(); 
    bool UpdateMesh();

    bool LoadModel();

    // removes geodes from root
    void RemoveGeodes();

    //Z Modelu
    // valid?
    bool IsValid(); 
    // draw normals
    void DrawNormals(float size);
    // draw WireFrame
    void DrawWireFrame(osg::Geode* geode);
    // add shaders
    bool AddShaders(const std::string& vertex, const std::string& pixel);
    // apply material
    bool ApplyMaterial(unsigned int mesh_id, unsigned int id, bool isVisible);	
    // loads shader
    bool LoadShaderSource(osg::Shader* obj, const std::string& file);
    size_t GetVertexSize();

    void UpdateWireFrame();
    void ResetWireFrame();

    void Update();



    // TODO: Fix this 
    ObjectService*          _pScene;
    // properly loaded?
    bool                    _properlyLoaded;
    // material id
    int                     _materialId;
    size_t                  _vertexSize;
    // Original mesh
    std::vector<SVertice*>  _meshBufferList;
    // root mesh
    std::vector<SVertice*>  _meshRootBufferList;


private:
    // create arrays of bones
    void CreateArrayOfJoints(SkeletonNode* bone);
    // draws one bone
    void DrawBone(osg::PositionAttitudeTransform* bone, const osg::Vec3d* parentPos, const osg::Quat* parentRot, osg::Geode* geode);
    // update bone
    void UpdateBone(osg::PositionAttitudeTransform* bone);
    void getTransformToGroup( osg::PositionAttitudeTransform* bone, const osg::Vec3d* parentPos, const osg::Quat* parentRot, osg::Group* group);
    // recalculates changes of skeleton
    void RecalculateChanges();

    osg::ref_ptr<osg::Geometry> DrawLine(const osg::Vec3d* startPos, const osg::Vec3d* endPos);
    osg::ref_ptr<osg::Geometry> DrawTriangle(const osg::Vec3d* startPos, const osg::Vec3d* endPos, const osg::Vec3d* vertexPos, 
        const osg::Vec3d* startPos2, const osg::Vec3d* endPos2, const osg::Vec3d* vertexPos2, bool isSelected);

    void drawGroup(osg::PositionAttitudeTransform* bone, const osg::Vec3d* parentPos, const osg::Quat* parentRot, osg::Geode* geode);

    static std::string                  _groupName;
    // animations
    std::map<std::string, Animation*>	_animations;
    // skeleton
    osg::ref_ptr<osg::Group>			_skeleton;
    // vector of pointers at bones
    std::vector<SkeletonNode*>*		    _joints;
    // geode of skeleton
    osg::ref_ptr<osg::Geode>			_skeletonGeode;
    // vector holding initial bone transformations
    STransformation*					_initialBones;
    // vector holding actual bones - transformed!
    STransformation*					_actualBones;
    // number of bones
    unsigned int						_numOfBones;

    std::string                         _selectedGroupName;
    // temporary array
    osg::Vec3d							_tempVectors[MAX_AFFECTING_BONES][2];
    // last played animation
    std::string							_lastPlayedAnimation;
    osg::ref_ptr<osg::Geode>			_wireFrameGeode;

 
protected:
    // Root Node 
    osg::ref_ptr<osg::Group>    _root;
    bool                        _isModelLoaded;
    // Meshes
    std::vector<geoderef>       _meshes;
};

#endif  // MODEL_H