#include "stdafx.h"
#include <KinematicModelLibrary/DegreeOfFreedom.h>
#include <KinematicModelLibrary/Joint.h>
#include <KinematicModelLibrary/Skeleton.h>
#include <KinematicModelLibrary/SkeletalModel.h>

using namespace hmAnimation;
using namespace std;

Skeleton::Skeleton(): 
    position(0.0,0.0,0.0),
    orientation(0.0,0.0,0.0),
    axisOrder(Axis::XYZ),
    rootName("root")
{
}
