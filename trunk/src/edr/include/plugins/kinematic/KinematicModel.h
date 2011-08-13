#ifndef __HEADER_GUARD_KINEMATIC__KINEMATICMODEL_H__
#define __HEADER_GUARD_KINEMATIC__KINEMATICMODEL_H__

#include <kinematiclib/SkeletalModel.h>
#include <kinematiclib/JointAnglesCollection.h>
#include <kinematiclib/VskParser.h>
#include <plugins/c3d/C3DChannels.h>


class KinematicModel
{
private:
    kinematic::JointAnglesCollectionPtr skeleton;
    MarkerCollectionPtr markers;
    
public:
    KinematicModel() {}

    // todo wprowadzic const
public:
    kinematic::JointAnglesCollectionPtr getSkeleton() const { return skeleton; }
    void setSkeleton(kinematic::JointAnglesCollectionPtr val) { skeleton = val; }
    MarkerCollectionPtr getMarkers() const { return markers; }
    void setMarkers(MarkerCollectionPtr val) { markers = val; }
};

typedef core::shared_ptr<KinematicModel> KinematicModelPtr;
typedef core::shared_ptr<const KinematicModel> KinematicModelConstPtr;

#endif  // __HEADER_GUARD_KINEMATIC__KINEMATICMODEL_H__