project(kinematic)
# nazwa
set(TARGET_TARGETNAME
	kinematic
)

add_definitions(-D__LIBRARY_DYNAMIC__)

#------------------------------------------------------------------------------

# nag³ówki prywatne
set(PRIVATE_H
	KinematicPlugin.h
	KinematicParser.h
	KinematicService.h
	ISchemeDrawer.h
	LineSchemeDrawer.h
	mainwindow.h
	OsgSchemeDrawer.h
	PointSchemeDrawer.h
	SkeletonViewerLogic.h
	UniqueCollection.h
)
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRIVATE_H})

# nag³ówki publiczne
string(REPLACE ${PROJECT_ROOT} ${PROJECT_INCLUDE_ROOT} HEADER_PATH ${PROJECT_SOURCE_DIR})
set(PUBLIC_H
	${HEADER_PATH}/Wrappers.h
	${HEADER_PATH}/skeletalVisualizationScheme.h
)
source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES ${PUBLIC_H})
set(TARGET_H ${PRIVATE_H} ${PUBLIC_H})

#------------------------------------------------------------------------------

# Ÿród³a
set(TARGET_SRC
	skeletalVisualizationScheme.cpp
	KinematicPlugin.cpp
	KinematicParser.cpp
	KinematicService.cpp
	LineSchemeDrawer.cpp
	mainwindow.cpp
	PointSchemeDrawer.cpp
	SkeletonViewerLogic.cpp
)
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_SRC})

# metadane generowane przez MOC
SET(TARGET_MOC_H
	mainwindow.h
)
QT4_WRAP_CPP(TARGET_MOC_SRC ${TARGET_MOC_H})
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_MOC_SRC})
set(TARGET_SRC ${TARGET_SRC} ${TARGET_MOC_SRC})

#------------------------------------------------------------------------------
# UI
set(TARGET_UI
	mainwindow.ui
)
QT4_WRAP_UI(TARGET_UI_H ${TARGET_UI})
source_group("${SOURCEGROUP_UI}" FILES ${TARGET_UI_H} ${TARGET_UI})
set(TARGET_H ${TARGET_H} ${TARGET_UI_H} ${TARGET_UI})

#------------------------------------------------------------------------------
# prekompilowany nag³ówek
set(PRECOMPILED_H PCH.h)
set(PRECOMPILED_SRC PCH.cpp)
SET_PRECOMPILED_HEADER(${PRECOMPILED_H} ${PRECOMPILED_SRC} "${TARGET_SRC}")
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRECOMPILED_H})
set(TARGET_H ${TARGET_H} ${PRECOMPILED_H})
source_group("${SOURCEGROUP_SOURCES}" FILES ${PRECOMPILED_SRC})
set(TARGET_SRC ${TARGET_SRC} ${PRECOMPILED_SRC})


#------------------------------------------------------------------------------
# tworzenie

# obsluga OpenGL
set(QT_USE_QTOPENGL TRUE) 
include(${QT_USE_FILE})

# kompilacja do wspó³dzielonej biblioteki
add_library(${TARGET_TARGETNAME} SHARED ${TARGET_SRC} ${TARGET_H})
#instalacja
install(TARGETS ${TARGET_TARGETNAME} DESTINATION bin COMPONENT kinematic)
#add_executable(${TARGET_TARGETNAME} ${TARGET_SRC} ${TARGET_H})
# nag³ówki
include_directories(${DEFAULT_PROJECT_INCLUDES})
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	

ON_PROJECT_ADDED(${TARGET_TARGETNAME})
