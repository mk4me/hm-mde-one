#ifndef SKELETON_NODE_H
#define SKELETON_NODE_H

#include <osg/Group>
#include <osg/PositionAttitudeTransform>
#include "AnimationGroup.h"

//--------------------------------------------------------------------------------------------------
// Adds to PositionAttitudeTransform field id
class SkeletonNode : public osg::PositionAttitudeTransform
{
public:
    SkeletonNode();
    void AddAnimation(osg::ref_ptr<CAnimationGroup> animation); 
    void SetId(int id); 
    // get id
    int  GetId(); 
    // number of animations
    unsigned int GetNumOfAnimations(); 
    // get animation
    const std::vector<osg::ref_ptr<CAnimationGroup> >* GetAnimations(); 

protected:
    virtual ~SkeletonNode();

private: 
    // Node ID
    int _id;
    // Lists of the animations for the current node
    std::vector<osg::ref_ptr<CAnimationGroup> >	_animations;

};

#endif