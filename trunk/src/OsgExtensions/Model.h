#ifndef NEW_MODEL_H
#define NEW_MODEL_H


#include <vector>
#include <string>

#include <osg/PositionAttitudeTransform>
#include <osg/Group>
#include "../include/IModel.h"
#include "../include/IMesh.h"

class Mesh;
class ISkeletonNode;
class SkeletonNode;
struct SSkeleton;
struct SFAnimation;

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
    virtual void* GetSkeletonGroup();

    virtual IMesh* GetMesh(int i = 0);
    virtual std::vector<IMesh* >& GetMeshList();
    virtual std::vector<ISkeletonNode* >* GetJoints();

    virtual SSkeleton* GetSkeleton();
    virtual SFAnimation* GetAnimation();

private:
    void CreateArrayOfJoints(SkeletonNode* bone);
 
    std::vector<ISkeletonNode* >* m_pJoints;
    SSkeleton* m_pSkeleton;
    SFAnimation* m_pAnimation;
    std::vector<IMesh* > m_meshList;
};


#endif //NEW_MODEL_H