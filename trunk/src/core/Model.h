#ifndef NEW_MODEL_H
#define NEW_MODEL_H


#include <vector>
#include <string>

#include <osg/PositionAttitudeTransform>
#include <osg/Group>
#include <osg/Texture2D>
#include <osg/Material>
#include <osgDB/ReadFile>

#include <core/IModel.h>
#include <core/IMesh.h>
#include <core/Skeleton.h>
#include <core/Frame.h>

class Mesh;
class ISkeletonNode;
class SkeletonNode;
struct Frame;
struct SMaterial;

#define MAX_AFFECTING_BONES 8

class Model:public IModel, public osg::Group
{
public:
    Model();
    virtual ~Model();

    virtual void Clear();
    virtual void AddMesh(IMesh* mesh);
    virtual void InicializeMesh();
    virtual void SetSkeleton(Skeleton* skeleton);
    virtual void SetAnimation(SkeletonAnimationList* animation);
    virtual void SetJoints(std::vector<ISkeletonNode* >* joints);
    virtual void ApplyMaterial(std::vector<SMaterial>* materialList, std::wstring textureDir);
    virtual void DrawModelBone();

    virtual void* GetSkeletonGroup();

    virtual IMesh* GetMesh(int i = 0);
    virtual std::vector<IMesh* >& GetMeshList();
    virtual std::vector<ISkeletonNode* >* GetJoints();
    virtual osg::ref_ptr<osg::Geode> GetSkeletonGeode();

    virtual Skeleton* GetSkeleton();
    virtual SkeletonAnimationList* GetAnimation();

    void DrawBone(Bone* bone, osg::Geode* geode);

    osg::ref_ptr<osg::Geometry> DrawLine(const osg::Vec3f* startPos, const osg::Vec3f* endPos, bool isSelected);

    osg::ref_ptr<osg::Geometry>  m_geometry;
    osg::ref_ptr<osg::Geode> m_spSkeletonGeode;

private:
    void CreateArrayOfJoints(SkeletonNode* bone);

    std::vector<ISkeletonNode* >* m_pJoints;
    Skeleton* m_pSkeleton;
    SkeletonAnimationList* m_pAnimation;
    std::vector<IMesh* > m_meshList;
};


#endif //NEW_MODEL_H