#ifndef __HEADER_GUARD_KINEMATIC__WRAPPERS_H__
#define __HEADER_GUARD_KINEMATIC__WRAPPERS_H__

#include <core/ObjectWrapper.h>
#include <kinematiclib/SkeletalModel.h>
#include <kinematiclib/JointAnglesCollection.h>

CORE_DEFINE_WRAPPER(kinematic::JointAnglesCollection, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
CORE_DEFINE_WRAPPER(kinematic::SkeletalData, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
CORE_DEFINE_WRAPPER(kinematic::SkeletalModel, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
CORE_DEFINE_WRAPPER(std::string, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);


#endif  // __HEADER_GUARD_KINEMATIC__WRAPPERS_H__
