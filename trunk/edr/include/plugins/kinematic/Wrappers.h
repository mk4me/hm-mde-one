#ifndef __HEADER_GUARD_KINEMATIC__WRAPPERS_H__
#define __HEADER_GUARD_KINEMATIC__WRAPPERS_H__

#include <core/ObjectWrapper.h>
#include <kinematiclib/SkeletalModel.h>
#include <kinematiclib/KinematicModel.h>
#include <plugins/kinematic/skeletalVisualizationScheme.h>

CORE_DEFINE_WRAPPER(kinematic::KinematicModel, utils::PtrPolicyBoost, utils::ClonePolicyNotImplemented);
CORE_DEFINE_WRAPPER(SkeletalVisualizationScheme, utils::PtrPolicyBoost, utils::ClonePolicyNotImplemented);

#endif  // __HEADER_GUARD_KINEMATIC__WRAPPERS_H__