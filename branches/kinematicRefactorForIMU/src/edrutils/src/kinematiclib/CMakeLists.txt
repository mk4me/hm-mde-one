BEGIN_PROJECT("static")

SET_PUBLIC_HEADERS(
	Joint.h
	SkeletalMappingScheme.h
	QuaternionManipulators.h
	SkeletalData.h
	Skeleton.h
	SkeletonState.h
)	

SET_SOURCE_FILES( 	
	SkeletalMappingScheme.cpp
	Skeleton.cpp
	SkeletonState.cpp
)

END_PROJECT()