# nazwa
set(TARGET_TARGETNAME skeletonViewer)


# Ÿród³a
set(TARGET_SRC 
	main.cpp
	mainwindow.cpp
	DisableableRenderer.cpp
	QOsgWidgets.cpp
	QOsgWidgetsDeprecated.cpp
	skeletonViewerLogic.cpp
	skeletalVisualizationScheme.cpp
	PointSchemeDrawer.cpp
	LineSchemeDrawer.cpp
)

# nag³ówki
set(TARGET_H
	mainwindow.h
	uniqueCollection.h
	DisableableRenderer.h
	QOsgWidgets.h
	QOsgWidgetsDeprecated.h
	skeletonViewerLogic.h
	skeletalVisualizationScheme.h
	ISchemeDrawer.h
	OsgSchemeDrawer.h
	PointSchemeDrawer.h
	LineSchemeDrawer.h
)
#------------------------------------------------------------------------------
# prekompilowany nag³ówek
set(PRECOMPILED_H PCH.h)
set(PRECOMPILED_SRC PCH.cpp)
SET_PRECOMPILED_HEADER(${PRECOMPILED_H} ${PRECOMPILED_SRC} "${TARGET_SRC}")
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRECOMPILED_H})
set(TARGET_H ${TARGET_H} ${PRECOMPILED_H})
source_group("${SOURCEGROUP_SOURCES}" FILES ${PRECOMPILED_SRC})
set(TARGET_SRC ${TARGET_SRC} ${PRECOMPILED_SRC})

# metadane generowane przez MOC
SET(TARGET_MOC_H
	mainwindow.h
	QOsgWidgets.h
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



# kompilacja do wspó³dzielonej biblioteki
add_executable(${TARGET_TARGETNAME} ${TARGET_SRC} ${TARGET_H})
# nag³ówki
include_directories( ${DEFAULT_PROJECT_INCLUDES} )
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})
# labelka
set_target_properties(${TARGET_TARGETNAME} PROPERTIES PROJECT_LABEL "Examples ${TARGET_TARGETNAME}")	