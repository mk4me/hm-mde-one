#ifndef __HEADER_GUARD_KINEMATIC__WRAPPERS_H__
#define __HEADER_GUARD_KINEMATIC__WRAPPERS_H__

#include <corelib/BaseDataTypes.h>
#include <threadingUtils/StreamData.h>
#include <kinematiclib/SkeletalModel.h>
#include <kinematiclib/JointAnglesCollection.h>
#include <utils/PtrPolicyOSG.h>
#include <osg/PositionAttitudeTransform>

//! Typ definiuj�cy indeksy na po��czonych punktach
typedef std::pair<unsigned int, unsigned int> SegmentRange;

//! Opis segmentu
struct SegmentDescriptor
{
	float length;			//! D�ugo�� po��czenia
	SegmentRange range;		//! indeksy punkt�w opisujacych segment
};

typedef std::vector<SegmentDescriptor> SegmentsDescriptors;

//! Strumie� danych szkieletu
typedef threadingUtils::StreamT<std::vector<osg::Vec3>> PointsCloudStream;
DEFINE_SMART_POINTERS(PointsCloudStream);

typedef threadingUtils::StreamT<std::vector<osg::Quat>> QuaternionStream;
DEFINE_SMART_POINTERS(QuaternionStream);

//! Dane szkieletu
struct SkeletonDataStream
{
	unsigned int jointsCount;				//! Ilosc joint�w w modelu
	PointsCloudStreamPtr jointsStream;		//! Strumie� dla pozycji joint�w
	QuaternionStreamPtr quatStream;			//! Strumie� globalnych kwaternion�w joint�w
	SegmentsDescriptors connections;	//! Schemat po��cze�
	std::map<std::string, int> segmentNames; 
};

DEFINE_SMART_POINTERS(SkeletonDataStream);

//tymczasowy typ dla BVH
typedef std::pair<kinematic::SkeletalModelPtr, kinematic::SkeletalDataPtr> BVHData;
DEFINE_SMART_POINTERS(BVHData);
DEFINE_WRAPPER(BVHData, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);

DEFINE_WRAPPER(kinematic::JointAnglesCollection, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
DEFINE_WRAPPER(kinematic::SkeletalData, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
DEFINE_WRAPPER(kinematic::SkeletalModel, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
DEFINE_WRAPPER(SkeletonDataStream, utils::PtrPolicyBoost, utils::ClonePolicyForbidden);
DEFINE_WRAPPER(std::string, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);
DEFINE_WRAPPER(osg::PositionAttitudeTransform, utils::PtrPolicyOSG, utils::ClonePolicyOsgCloneMethod);

#endif  // __HEADER_GUARD_KINEMATIC__WRAPPERS_H__
