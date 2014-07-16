#ifndef __HEADER_GUARD_KINEMATIC__WRAPPERS_H__
#define __HEADER_GUARD_KINEMATIC__WRAPPERS_H__

#include <corelib/BaseDataTypes.h>
#include <utils/StreamData.h>
#include <kinematiclib/SkeletalModel.h>
#include <kinematiclib/JointAnglesCollection.h>
#include <utils/PtrPolicyOSG.h>
#include <osg/PositionAttitudeTransform>

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
typedef utils::StreamT<std::vector<osg::Vec3>> PointsCloudStream;

DEFINE_SMART_POINTERS(PointsCloudStream);

//! Dane szkieletu
struct SkeletonDataStream
{
	unsigned int jointsCount;				//! Ilosc jointów w modelu
	PointsCloudStreamPtr jointsStream;		//! Strumieñ dla pozycji jointów
	SegmentsDescriptors connections;	//! Schemat po³¹czeñ
};

DEFINE_SMART_POINTERS(SkeletonDataStream);

DEFINE_WRAPPER(kinematic::JointAnglesCollection, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
DEFINE_WRAPPER(kinematic::SkeletalData, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
DEFINE_WRAPPER(kinematic::SkeletalModel, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
DEFINE_WRAPPER(SkeletonDataStream, utils::PtrPolicyBoost, utils::ClonePolicyForbidden);
DEFINE_WRAPPER(std::string, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);
DEFINE_WRAPPER(osg::PositionAttitudeTransform, utils::PtrPolicyOSG, utils::ClonePolicyOsgCloneMethod);

#endif  // __HEADER_GUARD_KINEMATIC__WRAPPERS_H__
