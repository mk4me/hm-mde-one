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
#include <osgutils/OsgSchemeDrawer.h>

//! Struktura zawiera wczytane z pliku uaktualnienia stanu szkieletu
struct SkeletonStates
{
	std::vector<kinematic::SkeletonState::NonRigidCompleteStateChange> frames;
	double frameTime;
	double getLength() const {
		return frameTime * frames.size();
	}
};
DEFINE_SMART_POINTERS(SkeletonStates);

//! Struktura szkieletu wraz z danymi
struct SkeletonWithStates
{
	SkeletonStatesConstPtr states;
	kinematic::SkeletonConstPtr skeleton;
	kinematic::SkeletonState::LinearizedNodesMapping nodesMapping;
	double scale = 1.0;
};
DEFINE_SMART_POINTERS(SkeletonWithStates);


typedef threadingUtils::StreamT<kinematic::SkeletonState::NonRigidCompleteStateChange> SkeletonStateStream;
DEFINE_SMART_POINTERS(SkeletonStateStream);

struct SkeletonWithStreamData
{
	kinematic::SkeletonConstPtr skeleton;
	kinematic::SkeletonState::LinearizedNodesMapping nodesMapping;
	SkeletonStateStreamPtr states;
};
DEFINE_SMART_POINTERS(SkeletonWithStreamData);



DEFINE_WRAPPER(SkeletonStates, utils::PtrPolicyStd, utils::ClonePolicyCopyConstructor);
DEFINE_WRAPPER(SkeletonWithStates, utils::PtrPolicyStd, utils::ClonePolicyCopyConstructor);
DEFINE_WRAPPER(acclaim::MotionData, utils::PtrPolicyStd, utils::ClonePolicyCopyConstructor);
DEFINE_WRAPPER(kinematic::Skeleton, utils::PtrPolicyStd, utils::ClonePolicyCopyConstructor);
DEFINE_WRAPPER(acclaim::Skeleton, utils::PtrPolicyStd, utils::ClonePolicyCopyConstructor);

//! Strumień danych szkieletu
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
	osgutils::SegmentsDescriptors connections;	//! Schemat po��cze�
	std::map<std::string, int> segmentNames; 
};

DEFINE_SMART_POINTERS(SkeletonDataStream);


DEFINE_WRAPPER(SkeletonDataStream, utils::PtrPolicyStd, utils::ClonePolicyForbidden);
DEFINE_WRAPPER(std::string, utils::PtrPolicyStd, utils::ClonePolicyCopyConstructor);
DEFINE_WRAPPER(osg::PositionAttitudeTransform, utils::PtrPolicyOSG, utils::ClonePolicyOsgCloneMethod);

#endif  // __HEADER_GUARD_KINEMATIC__WRAPPERS_H__
