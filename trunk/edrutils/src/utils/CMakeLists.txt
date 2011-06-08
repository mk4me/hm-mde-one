project(utils)
# nazwa
set(TARGET_TARGETNAME
	utils
)

# Opcje konfiguracji
CONFIG_OPTION(UTILS_ENABLE_PROFILER "Enable profiling?" OFF)
if(WIN32)
	CONFIG_OPTION(UTILS_ENABLE_LEAK_DETECTION_VLD "Enable leak detection with VLD? (http://sites.google.com/site/dmoulding/vld)" OFF)
endif()


#------------------------------------------------------------------------------
# nag³ówki prywatne
set(PRIVATE_H
)
#source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRIVATE_H})

# nag³ówki publiczne
string(REPLACE ${PROJECT_ROOT} ${PROJECT_INCLUDE_ROOT} HEADER_PATH ${PROJECT_SOURCE_DIR})
set(PUBLIC_H
	${HEADER_PATH}/Align.h
	${HEADER_PATH}/ClonePolicies.h
	${HEADER_PATH}/DataChannel.h
	${HEADER_PATH}/Pool.h
	${HEADER_PATH}/Enum.h
	${HEADER_PATH}/NamedTree.h
	${HEADER_PATH}/PtrPolicyRaw.h
	${HEADER_PATH}/PtrPolicyBoost.h
	${HEADER_PATH}/PtrPolicyOSG.h
	${HEADER_PATH}/PtrPolicyHelper.h
	${HEADER_PATH}/PtrWrapper.h
	${HEADER_PATH}/ObserverPattern.h
	${HEADER_PATH}/Macros.h
	${HEADER_PATH}/Utils.h
	${HEADER_PATH}/Debug.h
	${HEADER_PATH}/Profiler.h
	${HEADER_PATH}/System.h
	${HEADER_PATH}/LeakDetection.h
	${HEADER_PATH}/Push.h
	${HEADER_PATH}/Synchronized.h
)
source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES ${PUBLIC_H})
set(TARGET_H ${PRIVATE_H} ${PUBLIC_H})


#------------------------------------------------------------------------------
# konfiguracja

# Kopiujemy konfiguracja
configure_file( "Config.h.in" "${PROJECT_BINARY_DIR}/Config.h" )
set(TARGET_H ${TARGET_H} "${PROJECT_BINARY_DIR}/Config.h" Config.h.in)
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES Config.h.in)
source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES "${PROJECT_BINARY_DIR}/Config.h")

#------------------------------------------------------------------------------
# Ÿród³a
set(TARGET_SRC 
	Enum.cpp
	Debug.cpp
	NamedTree.cpp
	Profiler.cpp
)
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_SRC})
set(TARGET_SRC ${TARGET_SRC})

#------------------------------------------------------------------------------
# tworzenie

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
install(TARGETS ${TARGET_TARGETNAME} DESTINATION lib)
install(FILES ${PUBLIC_H} DESTINATION include/${TARGET_TARGETNAME})
install(FILES "${PROJECT_BINARY_DIR}/Config.h" DESTINATION include/${TARGET_TARGETNAME})
