#ifndef SKELETON_NODE_H
#define SKELETON_NODE_H

#include <osg/Group>
#include <osg/PositionAttitudeTransform>
#include <core/IAnimationGroup.h>
#include <core/ISkeletonNode.h>


//--------------------------------------------------------------------------------------------------
// Adds to PositionAttitudeTransform field id
class SkeletonNode :public ISkeletonNode, public osg::PositionAttitudeTransform
{
public:
    SkeletonNode();
    virtual void AddAnimation(IAnimationGroup* animation); 
    virtual void SetAttitude(const osg::Quat& quat);
    virtual void SetPosition(osg::Vec3d &pos);
    virtual ISkeletonNode* GetParent(int i);
    virtual void SetId(int id); 
  
    virtual unsigned int GetNumOfAnimations(); // number of animations   
    virtual int  GetId(); // get id

    virtual const std::vector<IAnimationGroup* >* GetAnimations(); // get animation
    virtual const osg::Vec3d& GetPosition() const;
    virtual const osg::Quat& GetAttitude() const;

protected:
    virtual ~SkeletonNode();

private: 
    // Node ID
    int _id;
    // Lists of the animations for the current node
    std::vector<IAnimationGroup* >	_animations;

};

#endif