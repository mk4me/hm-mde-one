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

class Mesh;
class ISkeletonNode;
class SkeletonNode;
struct SSkeleton;
struct SFAnimation;
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
    virtual void SetSkeleton(SSkeleton* skeleton);
    virtual void SetAnimation(SFAnimation* animation);
    virtual void SetJoints(std::vector<ISkeletonNode* >* joints);
    virtual void ApplyMaterial(std::vector<SMaterial>* materialList, std::wstring textureDir);
    virtual void* GetSkeletonGroup();

    virtual IMesh* GetMesh(int i = 0);
    virtual std::vector<IMesh* >& GetMeshList();
    virtual std::vector<ISkeletonNode* >* GetJoints();

    virtual SSkeleton* GetSkeleton();
    virtual SFAnimation* GetAnimation();

    void DrawBone(osg::PositionAttitudeTransform* bone, const osg::Vec3d* parentPos, const osg::Quat* parentRot, osg::Geode* geode);
    osg::ref_ptr<osg::Geometry> DrawTriangle(const osg::Vec3d* startPos, const osg::Vec3d* endPos, const osg::Vec3d* vertexPos, 
        const osg::Vec3d* startPos2, const osg::Vec3d* endPos2, const osg::Vec3d* vertexPos2, bool isSelected);

    osg::ref_ptr<osg::Geometry> DrawLine(const osg::Vec3d* startPos, const osg::Vec3d* endPos);

	virtual void DrawModelBone();

	osg::ref_ptr<osg::Geometry>  m_geometry;

	osg::ref_ptr<osg::Geode> _skeletonGeode;
private:
    void CreateArrayOfJoints(SkeletonNode* bone);

    std::vector<ISkeletonNode* >* m_pJoints;
    SSkeleton* m_pSkeleton;
    SFAnimation* m_pAnimation;
    std::vector<IMesh* > m_meshList;
};


#endif //NEW_MODEL_H