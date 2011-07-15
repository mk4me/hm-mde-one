#ifndef __HEADER_GUARD_KINEMATIC__KINEMATICMODEL_H__
#define __HEADER_GUARD_KINEMATIC__KINEMATICMODEL_H__

#include <kinematiclib/SkeletalModel.h>
#include <kinematiclib/KinematicModel.h>
#include <plugins/c3d/C3DChannels.h>

class KinematicModel
{
private:
    kinematic::KinematicSkeletonPtr skeleton;
    MarkerSetPtr markers;
    
public:
    KinematicModel() {}

    // todo wprowadzic const
public:
    kinematic::KinematicSkeletonPtr getSkeleton() const { return skeleton; }
    void setSkeleton(kinematic::KinematicSkeletonPtr val) { skeleton = val; }
    MarkerSetPtr getMarkers() const { return markers; }
    void setMarkers(MarkerSetPtr val) { markers = val; }
};

typedef core::shared_ptr<KinematicModel> KinematicModelPtr;
typedef core::shared_ptr<const KinematicModel> KinematicModelConstPtr;

#endif  // __HEADER_GUARD_KINEMATIC__KINEMATICMODEL_H__