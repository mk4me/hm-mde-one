#ifndef __HEADER_GUARD_KINEMATIC__WRAPPERS_H__
#define __HEADER_GUARD_KINEMATIC__WRAPPERS_H__

#include <core/ObjectWrapper.h>
#include <kinematiclib/SkeletalModel.h>
#include <kinematiclib/KinematicModel.h>

CORE_DEFINE_WRAPPER(kinematic::KinematicModel, utils::PtrPolicyBoost);

#endif  // __HEADER_GUARD_KINEMATIC__WRAPPERS_H__