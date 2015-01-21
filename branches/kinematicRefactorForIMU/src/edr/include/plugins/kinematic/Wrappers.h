#ifndef __HEADER_GUARD_KINEMATIC__WRAPPERS_H__
#define __HEADER_GUARD_KINEMATIC__WRAPPERS_H__

#include <corelib/BaseDataTypes.h>
#include <threadingUtils/StreamData.h>
#include <utils/PtrPolicyOSG.h>
#include <utils/PtrPolicyStd.h>
#include <osg/PositionAttitudeTransform>
#include <acclaimformatslib/Skeleton.h>
#include <acclaimformatslib/MotionData.h>
#include <kinematiclib/Skeleton.h>
#include <kinematiclib/SkeletonState.h>


struct SkeletonStates
{
	std::vector<kinematic::SkeletonState::NonRigidCompleteStateChange> frames;
	//std::vector<std::string> jointNames;
	double frameTime;
	double getLength() const {
		return frameTime * frames.size();
	}
};
DEFINE_SMART_POINTERS(SkeletonStates);

struct SkeletonWithStates
{
	SkeletonStatesConstPtr states;
	kinematic::SkeletonConstPtr skeleton;
	kinematic::SkeletonState::LinearizedNodesMapping nodesMapping;
};
DEFINE_SMART_POINTERS(SkeletonWithStates);


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

DEFINE_WRAPPER(SkeletonStates, utils::PtrPolicyStd, utils::ClonePolicyCopyConstructor);
DEFINE_WRAPPER(SkeletonWithStates, utils::PtrPolicyStd, utils::ClonePolicyCopyConstructor);
DEFINE_WRAPPER(acclaim::MotionData, utils::PtrPolicyStd, utils::ClonePolicyCopyConstructor);
DEFINE_WRAPPER(kinematic::Skeleton, utils::PtrPolicyStd, utils::ClonePolicyCopyConstructor);
DEFINE_WRAPPER(acclaim::Skeleton, utils::PtrPolicyStd, utils::ClonePolicyCopyConstructor);
DEFINE_WRAPPER(SkeletonDataStream, utils::PtrPolicyStd, utils::ClonePolicyForbidden);
DEFINE_WRAPPER(std::string, utils::PtrPolicyStd, utils::ClonePolicyCopyConstructor);
DEFINE_WRAPPER(osg::PositionAttitudeTransform, utils::PtrPolicyOSG, utils::ClonePolicyOsgCloneMethod);

#endif  // __HEADER_GUARD_KINEMATIC__WRAPPERS_H__
