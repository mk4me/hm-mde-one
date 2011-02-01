#ifndef I_ANIMATION_NODE_H
#define I_ANIMATION_NODE_H


#include <osg/Vec3d>
#include <osg/Quat>

//////////////////////////////////////////////////////////////////////////
// represents node holding animation - one frame
class IAnimationNode
{
public :
    virtual ~IAnimationNode() {}
    virtual const double GetTime() = 0;

    virtual const osg::Vec3d& GetPosition() const = 0;
    virtual const osg::Quat& GetAttitude() const = 0;

    virtual void SetPosition(osg::Vec3d& pos) = 0;
    virtual void SetAttitude(osg::Quat& quat) = 0;
};

#endif //I_ANIMATION_NODE_H