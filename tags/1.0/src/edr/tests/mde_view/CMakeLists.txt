BEGIN_PROJECT("executable")

FILE(GLOB objFiles  "${CMAKE_BINARY_DIR}/src/mde_view/mde_view.dir/Debug/*.obj")

list(REMOVE_ITEM objFiles "${CMAKE_BINARY_DIR}/src/mde_view/mde_view.dir/Debug/main.obj")

SET_SOURCE_FILES(
	test.cpp
	AnalisisModelTest.cpp
	VisualizerContextTest.cpp
)

SET_PRIVATE_HEADERS(
	AnalisisModelTest.h
	VisualizerContextTest.h
)

SET_MOC_FILES (
	VisualizerContextTest.h
)

set (TARGET_OBJ ${objFiles})

END_PROJECT()