BEGIN_PROJECT("static")

SET_PUBLIC_HEADERS(
	AmcParser.h
	AsfParser.h
	BvhParser.h
	VskParser.h
	DegreeOfFreedom.h
	hmException.h
	Joint.h
	hAnimJoint.h
	hAnimBone.h
	hAnimSkeleton.h
	SkeletalMappingScheme.h
	QuaternionManipulators.h
	JointAnglesCollection.h
	SkeletalModel.h
	SkeletalData.h
	SkeletalParsers.h
	Skeleton.h
	Logger.h
)	

SET_PRIVATE_HEADERS(
	stdafx.h
)

SET_SOURCE_FILES( 
	AmcParser.cpp
	AsfParser.cpp
	BvhParser.cpp
	VskParser.cpp
	DegreeOfFreedom.cpp
	hmException.cpp
	Joint.cpp
	hAnimJoint.cpp
	hAnimSkeleton.cpp
	SkeletalMappingScheme.cpp
	JointAnglesCollection.cpp
	SkeletalModel.cpp
	Skeleton.cpp
	stdafx.cpp
)

SET_PRECOMPILED_HEADER(stdafx.h stdafx.cpp)

END_PROJECT()