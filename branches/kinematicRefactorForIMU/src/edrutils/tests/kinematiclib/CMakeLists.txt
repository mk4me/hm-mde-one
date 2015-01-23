BEGIN_PROJECT("executable")

SET_SOURCE_FILES(
	test.cpp
	JointTest.cpp
	QuaternionManipulatorsTest.cpp
	SkeletalDataTest.cpp
	SkeletonTest.cpp
	SkeletonStateTest.cpp
)

SET_PRIVATE_HEADERS(
	JointTest.h
	QuaternionManipulatorsTest.h
	SkeletalDataTest.h
	SkeletonTest.h
	SkeletonStateTest.h
)


#TODO moze warto to jakos uniwersalnie zrobic..
file(COPY "testFiles" DESTINATION "${CMAKE_BINARY_DIR}/tests/kinematiclib")

END_PROJECT()