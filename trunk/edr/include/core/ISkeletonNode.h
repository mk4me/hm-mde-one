#ifndef I_SKELETON_NODE_H
#define I_SKELETON_NODE_H

#include <osg/Vec3d>
#include <osg/Quat>
#include <vector>

class IAnimationGroup;

class ISkeletonNode
{
public:
    virtual ~ISkeletonNode() {};

    virtual void AddAnimation(IAnimationGroup* animation) = 0;
    virtual void SetAttitude(const osg::Quat& quat) = 0;
    virtual void SetPosition(osg::Vec3d &pos) = 0;
    virtual ISkeletonNode* GetParent(int i) = 0;
    virtual void SetId(int id) = 0;

    virtual unsigned int GetNumOfAnimations() = 0; // number of animations
    virtual int  GetId() = 0; // get id

    virtual const std::vector<IAnimationGroup* >* GetAnimations() = 0; // get animation
    virtual const osg::Vec3d& GetPosition() const = 0;
    virtual const osg::Quat& GetAttitude() const = 0;
};

#endif // I_SKELETON_NODE_H