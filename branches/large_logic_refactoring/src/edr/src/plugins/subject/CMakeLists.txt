BEGIN_PROJECT("dynamic")

SET_PUBLIC_HEADERS(
	ISubject.h
	ISession.h
	IMotion.h
	IDataFilter.h
	Types.h
	ISubjectService.h
	IntegumentarySystem.h
	ISubjectSystem.h
	MuscularSystem.h
	SkeletalSystem.h
	SolidSystem.h
	SubjectSystemConfiguration.h
)

SET_PRIVATE_HEADERS(
	SubjectService.h
	Motion.h
	Session.h
	Subject.h
	SubjectPCH.h
)

SET_SOURCE_FILES (
	Motion.cpp
	Session.cpp
	Subject.cpp
	SubjectService.cpp
	SubjectPlugin.cpp
	SubjectPCH.cpp
)

SET_PRECOMPILED_HEADER( SubjectPCH.h SubjectPCH.cpp)

END_PROJECT()