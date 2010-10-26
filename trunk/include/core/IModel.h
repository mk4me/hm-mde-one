#ifndef I_MODEL_H
#define I_MODEL_H


#include <vector>
#include <string>

class Mesh;
class IMesh;
class ISkeletonNode;
class Skeleton;
struct Frame;
struct SMaterial;

class IModel
{
public:
    virtual ~IModel() {};

    virtual void Clear() = 0;
    virtual void AddMesh(IMesh* mesh) = 0;
    virtual void InicializeMesh() = 0;
    virtual void SetSkeleton(Skeleton* skeleton) = 0;
    virtual void SetAnimation(Frame* animation) = 0;
    virtual void SetJoints(std::vector<ISkeletonNode* >* joints) = 0;
    virtual void ApplyMaterial(std::vector<SMaterial>* materialList, std::wstring textureDir) = 0;
    virtual void* GetSkeletonGroup() = 0;

    virtual IMesh* GetMesh(int i = 0) = 0;
    virtual std::vector<IMesh* >& GetMeshList() = 0;

    virtual Skeleton* GetSkeleton() = 0;
    virtual Frame* GetAnimation() = 0;
    virtual std::vector<ISkeletonNode* >* GetJoints() = 0;
    virtual osg::ref_ptr<osg::Geode> GetSkeletonGeode() = 0;

	virtual void DrawModelBone() = 0;
};


#endif //I_MODEL_H