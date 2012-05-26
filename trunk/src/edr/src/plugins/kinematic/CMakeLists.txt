project(kinematic)
# nazwa
set(TARGET_TARGETNAME
	plugin_kinematic
)

#------------------------------------------------------------------------------

# nag³ówki prywatne
set(PRIVATE_H
	KinematicParser.h
	AsfParser.h
	KinematicVisualizer.h
	ISchemeDrawer.h
	LineSchemeDrawer.h
	OsgSchemeDrawer.h
	PointSchemeDrawer.h
	GLPointSchemeDrawer.h
	GLLineSchemeDrawer.h
	UniqueCollection.h
	SchemeDrawerContainer.h
	GRFSerie.h
	MarkerSerie.h
	KinematicSerie.h
	SkeletonSerie.h
	TrajectoriesDialog.h
	TrajectoriesDrawer.h
	SchemeDialog.h
	PickHandler.h
	Manipulators.h
	SkeletalVisualizationScheme.h
	VisualizationScheme.h
	MarkersVisualizationScheme.h
)
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRIVATE_H})

# nag³ówki publiczne
string(REPLACE ${PROJECT_ROOT} ${PROJECT_INCLUDE_ROOT} HEADER_PATH ${PROJECT_SOURCE_DIR})
set(PUBLIC_H
	${HEADER_PATH}/Wrappers.h
	
)
source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES ${PUBLIC_H})
set(TARGET_H ${PRIVATE_H} ${PUBLIC_H})

#------------------------------------------------------------------------------

# Ÿród³a
set(TARGET_SRC
	KinematicPlugin.cpp
	AsfParser.cpp
	KinematicParser.cpp
	KinematicSerie.cpp
	KinematicVisualizer.cpp
	LineSchemeDrawer.cpp
	PointSchemeDrawer.cpp
	GLPointSchemeDrawer.cpp
	GLLineSchemeDrawer.cpp
	SchemeDrawerContainer.cpp
	GRFSerie.cpp
	MarkerSerie.cpp
	SkeletonSerie.cpp
	TrajectoriesDialog.cpp
	TrajectoriesDrawer.cpp
	SchemeDialog.cpp
	PickHandler.cpp
	Manipulators.cpp
	SkeletalVisualizationScheme.cpp
	VisualizationScheme.cpp
	MarkersVisualizationScheme.cpp
)
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_SRC})

# metadane generowane przez MOC
SET(TARGET_MOC_H
	KinematicVisualizer.h
	SkeletonSerie.h
	MarkerSerie.h
	TrajectoriesDialog.h
	SchemeDialog.h
)
QT4_WRAP_CPP(TARGET_MOC_SRC ${TARGET_MOC_H})
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_MOC_SRC})
set(TARGET_SRC ${TARGET_SRC} ${TARGET_MOC_SRC})

#------------------------------------------------------------------------------
set(TARGET_UI
	ui/TrajectoriesDialog.ui
)
QT4_WRAP_UI(TARGET_UI_H ${TARGET_UI})
source_group("${SOURCEGROUP_UI}" FILES ${TARGET_UI_H} ${TARGET_UI})
set(TARGET_H ${TARGET_H} ${TARGET_UI_H} ${TARGET_UI})

#------------------------------------------------------------------------------
# RC
set(TARGET_RC
	ui/Icons.qrc
)
QT4_ADD_RESOURCES(TARGET_RCC_SRC ${TARGET_RC})
source_group("${SOURCEGROUP_UI}" FILES ${TARGET_RC} ${TARGET_RCC_SRC})
set(TARGET_SRC ${TARGET_SRC} ${TARGET_RCC_SRC} ${TARGET_RC})

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
install(TARGETS ${TARGET_TARGETNAME} RUNTIME DESTINATION bin COMPONENT kinematic)
#add_executable(${TARGET_TARGETNAME} ${TARGET_SRC} ${TARGET_H})
# nag³ówki
include_directories(${DEFAULT_PROJECT_INCLUDES})
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	

ON_PROJECT_ADDED(${TARGET_TARGETNAME})
