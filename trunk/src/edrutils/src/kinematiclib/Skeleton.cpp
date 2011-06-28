#include "stdafx.h"
#include <kinematiclib/DegreeOfFreedom.h>
#include <kinematiclib/Joint.h>
#include <kinematiclib/Skeleton.h>
#include <kinematiclib/SkeletalModel.h>

using namespace std;

namespace kinematic {

Skeleton::Skeleton():
    position(0.0, 0.0, 0.0),
    orientation(0.0, 0.0, 0.0),
    axisOrder(Axis::XYZ),
    dataOrder(Axis::XYZ),
    rootName("root")
{
}

} // kinematic