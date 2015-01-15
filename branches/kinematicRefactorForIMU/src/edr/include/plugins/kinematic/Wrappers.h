#ifndef __HEADER_GUARD_KINEMATIC__WRAPPERS_H__
#define __HEADER_GUARD_KINEMATIC__WRAPPERS_H__

#include <corelib/BaseDataTypes.h>
#include <threadingUtils/StreamData.h>
//#include <kinematiclib/SkeletalModel.h>
#include <kinematiclib/JointAnglesCollection.h>
#include <utils/PtrPolicyOSG.h>
#include <utils/PtrPolicyStd.h>
#include <osg/PositionAttitudeTransform>
#include <acclaimformatslib/Skeleton.h>
#include <acclaimformatslib/MotionData.h>
#include <kinematiclib/Skeleton.h>
#include <kinematiclib/SkeletonState.h>

//! Typ definiuj¹cy indeksy na po³¹czonych punktach
typedef std::pair<unsigned int, unsigned int> SegmentRange;

//! Opis segmentu
struct SegmentDescriptor
{
	float length;			//! D³ugoœæ po³¹czenia
	SegmentRange range;		//! indeksy punktów opisujacych segment
};

typedef std::vector<SegmentDescriptor> SegmentsDescriptors;

//! Strumieñ danych szkieletu
typedef threadingUtils::StreamT<std::vector<osg::Vec3>> PointsCloudStream;
DEFINE_SMART_POINTERS(PointsCloudStream);

typedef threadingUtils::StreamT<std::vector<osg::Quat>> QuaternionStream;
DEFINE_SMART_POINTERS(QuaternionStream);

//! Dane szkieletu
struct SkeletonDataStream
{
	unsigned int jointsCount;				//! Ilosc jointów w modelu
	PointsCloudStreamPtr jointsStream;		//! Strumieñ dla pozycji jointów
	QuaternionStreamPtr quatStream;			//! Strumieñ globalnych kwaternionów jointów
	SegmentsDescriptors connections;	//! Schemat po³¹czeñ
	std::map<std::string, int> segmentNames; 
};

DEFINE_SMART_POINTERS(SkeletonDataStream);


DEFINE_WRAPPER(kinematic::JointAnglesCollection, utils::PtrPolicyStd, utils::ClonePolicyVirtualCloneMethod);
DEFINE_WRAPPER(kinematic::SkeletonStates, utils::PtrPolicyStd, utils::ClonePolicyCopyConstructor);
DEFINE_WRAPPER(kinematic::SkeletonWithStates, utils::PtrPolicyStd, utils::ClonePolicyCopyConstructor);
DEFINE_WRAPPER(acclaim::MotionData, utils::PtrPolicyStd, utils::ClonePolicyCopyConstructor);
DEFINE_WRAPPER(kinematic::Skeleton, utils::PtrPolicyStd, utils::ClonePolicyCopyConstructor);
DEFINE_WRAPPER(acclaim::Skeleton, utils::PtrPolicyStd, utils::ClonePolicyCopyConstructor);
DEFINE_WRAPPER(SkeletonDataStream, utils::PtrPolicyStd, utils::ClonePolicyForbidden);
DEFINE_WRAPPER(std::string, utils::PtrPolicyStd, utils::ClonePolicyCopyConstructor);
DEFINE_WRAPPER(osg::PositionAttitudeTransform, utils::PtrPolicyOSG, utils::ClonePolicyOsgCloneMethod);

#endif  // __HEADER_GUARD_KINEMATIC__WRAPPERS_H__
