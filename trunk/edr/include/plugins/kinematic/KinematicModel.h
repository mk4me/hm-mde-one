#ifndef __HEADER_GUARD_KINEMATIC__KINEMATICMODEL_H__
#define __HEADER_GUARD_KINEMATIC__KINEMATICMODEL_H__

#include <kinematiclib/SkeletalModel.h>
#include <kinematiclib/KinematicModel.h>
#include <core/C3DParserEx.h>

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

typedef boost::shared_ptr<KinematicModel> KinematicModelPtr;
typedef boost::shared_ptr<const KinematicModel> KinematicModelConstPtr;

#endif  // __HEADER_GUARD_KINEMATIC__KINEMATICMODEL_H__