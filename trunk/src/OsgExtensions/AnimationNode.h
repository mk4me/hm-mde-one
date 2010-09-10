#ifndef ANIMATION_NODE_H
#define ANIMATION_NODE_H


#include <osg/PositionAttitudeTransform>
#include "IAnimationNode.h"


//////////////////////////////////////////////////////////////////////////
// represents node holding animation - one frame
class CAnimationNode : public IAnimationNode, public osg::PositionAttitudeTransform
{
public :
    CAnimationNode(double time);
	~CAnimationNode() {}

	virtual const double GetTime() { return _time; }
    virtual const osg::Vec3d& GetPosition() const;
    virtual const osg::Quat& GetAttitude() const;

    virtual void SetPosition(osg::Vec3d& pos);
    virtual void SetAttitude(osg::Quat& quat);

private:
    double _time;
};

#endif


