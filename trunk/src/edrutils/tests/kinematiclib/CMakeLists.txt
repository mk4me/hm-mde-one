BEGIN_PROJECT("executable")

SET_SOURCE_FILES(
	test.cpp
	AsfParserTest.cpp
	AmcParserTest.cpp
	BvhParserTest.cpp
	MiscTest.cpp
	KinematicModelTest.cpp
)

SET_PRIVATE_HEADERS(
	AsfParserTest.h
	AmcParserTest.h
	BvhParserTest.h
	MiscTest.h
	KinematicModelTest.h
)

END_PROJECT()