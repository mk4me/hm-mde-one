project(timeline)
set(TARGET_TARGETNAME timeline)

# obs³uga QT
set(QT_USE_QTOPENGL TRUE)
include(${QT_USE_FILE})

add_definitions(-D__LIBRARY_DYNAMIC__)

#------------------------------------------------------------------------------

# nag³ówki
string(REPLACE ${EDR_ROOT} ${EDR_INCLUDE_ROOT} HEADER_PATH ${PROJECT_SOURCE_DIR})
set(PUBLIC_H
	${HEADER_PATH}/TimelineService.h
	${HEADER_PATH}/Stream.h
	${HEADER_PATH}/ITimeline.h
	${HEADER_PATH}/Controller.h
	${HEADER_PATH}/View.h
	${HEADER_PATH}/Model.h
	${HEADER_PATH}/State.h
)
source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES ${PUBLIC_H})
set(PRIVATE_H
	TimelineWidget.h
	TimelineViewOSG.h
)
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRIVATE_H})
set(TARGET_H ${PRIVATE_H} ${PUBLIC_H})

#------------------------------------------------------------------------------

# Ÿród³a
set(TARGET_SRC
	TimelineService.cpp
	TimelinePlugin.cpp
	TimelineWidget.cpp
	Controller.cpp
	Model.cpp
	Stream.cpp
)
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_SRC})

SET(TARGET_MOC_H
	TimelineWidget.h
)
QT4_WRAP_CPP(TARGET_MOC_SRC ${TARGET_MOC_H})
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_MOC_SRC})
set(TARGET_SRC ${TARGET_SRC} ${TARGET_MOC_SRC})

#------------------------------------------------------------------------------
# UI
SET(TARGET_UI
	ui/TimelineWidget.ui
)
QT4_WRAP_UI(TARGET_UI_H ${TARGET_UI})
source_group("${SOURCEGROUP_UI}" FILES ${TARGET_UI_H} ${TARGET_UI})
set(TARGET_H ${TARGET_H} ${TARGET_UI} ${TARGET_UI_H})


#------------------------------------------------------------------------------
# prekompilowany nag³ówek
set(PRECOMPILED_H TimelinePCH.h)
set(PRECOMPILED_SRC TimelinePCH.cpp)
SET_PRECOMPILED_HEADER(${PRECOMPILED_H} ${PRECOMPILED_SRC} "${TARGET_SRC}")
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRECOMPILED_H})
set(TARGET_H ${TARGET_H} ${PRECOMPILED_H})
source_group("${SOURCEGROUP_SOURCES}" FILES ${PRECOMPILED_SRC})
set(TARGET_SRC ${TARGET_SRC} ${PRECOMPILED_SRC})

#------------------------------------------------------------------------------
# tworzenie

# kompilacja do wspó³dzielonej biblioteki
add_library(${TARGET_TARGETNAME} SHARED ${TARGET_SRC} ${TARGET_H})
# nag³ówki
include_directories( ${DEFAULT_PROJECT_INCLUDES} )
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	

# ¿eby mo¿na by³o u¿yæ tej biblioteki w makrach
ON_PROJECT_ADDED(${TARGET_TARGETNAME})