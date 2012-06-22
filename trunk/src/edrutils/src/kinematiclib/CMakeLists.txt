project(kinematiclib)
# nazwa
set(TARGET_TARGETNAME kinematiclib)

# nag³ówki prywatne
set(PRIVATE_H
	#stdafx.h
)
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRIVATE_H})

# nag³ówki publiczne
string(REPLACE ${PROJECT_ROOT} ${PROJECT_INCLUDE_ROOT} HEADER_PATH ${PROJECT_SOURCE_DIR})
set(PUBLIC_H
	${HEADER_PATH}/AmcParser.h
	${HEADER_PATH}/AsfParser.h
	${HEADER_PATH}/BvhParser.h
	${HEADER_PATH}/VskParser.h
	${HEADER_PATH}/DegreeOfFreedom.h
	${HEADER_PATH}/hmException.h
	${HEADER_PATH}/Joint.h
	${HEADER_PATH}/hAnimJoint.h
	${HEADER_PATH}/hAnimBone.h
	${HEADER_PATH}/hAnimSkeleton.h
	${HEADER_PATH}/SkeletalMappingScheme.h
	${HEADER_PATH}/QuaternionManipulators.h
	${HEADER_PATH}/JointAnglesCollection.h
	${HEADER_PATH}/SkeletalModel.h
	${HEADER_PATH}/SkeletalData.h
	${HEADER_PATH}/SkeletalParsers.h
	${HEADER_PATH}/Skeleton.h
	${HEADER_PATH}/Logger.h
)
source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES ${PUBLIC_H})
set(TARGET_H ${PRIVATE_H} ${PUBLIC_H})

#------------------------------------------------------------------------------
# konfiguracja

# Kopiujemy konfiguracja
#configure_file( "Config.h.in" "${PROJECT_BINARY_DIR}/Config.h" )
#set(TARGET_H ${TARGET_H} "${PROJECT_BINARY_DIR}/Config.h" Config.h.in)
#source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES Config.h.in)
#source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES "${PROJECT_BINARY_DIR}/Config.h")

#------------------------------------------------------------------------------


# Ÿród³a
set(TARGET_SRC 
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
)
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_SRC})
#set(TARGET_SRC ${TARGET_SRC})

#------------------------------------------------------------------------------
# prekompilowany nag³ówek
set(PRECOMPILED_H stdafx.h)
set(PRECOMPILED_SRC stdafx.cpp)
SET_PRECOMPILED_HEADER(${PRECOMPILED_H} ${PRECOMPILED_SRC} "${TARGET_SRC}")
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRECOMPILED_H})
set(TARGET_H ${TARGET_H} ${PRECOMPILED_H})
source_group("${SOURCEGROUP_SOURCES}" FILES ${PRECOMPILED_SRC})
set(TARGET_SRC ${TARGET_SRC} ${PRECOMPILED_SRC})


# kompilacja do wspó³dzielonej biblioteki
add_library(${TARGET_TARGETNAME} STATIC ${TARGET_SRC} ${TARGET_H})
# nag³ówki
include_directories( ${DEFAULT_PROJECT_INCLUDES} )
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	
# nazwa
set_target_properties(${TARGET_TARGETNAME} PROPERTIES PROJECT_LABEL "Library ${TARGET_TARGETNAME}")

# ¿eby mo¿na by³o u¿yæ tej biblioteki w makrach
ON_PROJECT_ADDED(${TARGET_TARGETNAME})
# instalacja
install(TARGETS ${TARGET_TARGETNAME} RUNTIME DESTINATION bin LIBRARY DESTINATION lib/${PROJECT_NAME} ARCHIVE DESTINATION lib/${PROJECT_NAME} COMPONENT ${PROJECT_NAME})
install(FILES ${PUBLIC_H} DESTINATION include/${TARGET_TARGETNAME})
#install(FILES "${PROJECT_BINARY_DIR}/Config.h" DESTINATION include/${TARGET_TARGETNAME})
