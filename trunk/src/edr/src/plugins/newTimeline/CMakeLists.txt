project(newTimeline)
set(TARGET_TARGETNAME plugin_newTimeline)

# obs³uga QT
set(QT_USE_QTOPENGL TRUE)
include(${QT_USE_FILE})

#------------------------------------------------------------------------------

# nag³ówki
string(REPLACE ${PROJECT_ROOT} ${PROJECT_INCLUDE_ROOT} HEADER_PATH ${PROJECT_SOURCE_DIR})
set(PUBLIC_H
	${HEADER_PATH}/ITimelineService.h
	${HEADER_PATH}/TimeSliderWidget.h
	${HEADER_PATH}/TimelineControlsWidget.h
	)
source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES ${PUBLIC_H})
set(PRIVATE_H
	TimelineService.h
	TimelineWidget.h
	ChannelWidget.h
	ChannelsTreeItem.h
	ChannelCheckbox.h
)
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRIVATE_H})
set(TARGET_H ${PRIVATE_H} ${PUBLIC_H})

#------------------------------------------------------------------------------

# Ÿród³a
set(TARGET_SRC
	TimelineWidget.cpp
	ChannelWidget.cpp
	ChannelsTreeItem.cpp
	ChannelCheckbox.cpp
	TimelineService.cpp
	TimelinePlugin.cpp
	TimeSliderWidget.cpp
	TimelineControlsWidget.cpp
)
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_SRC})

SET(TARGET_MOC_H
	TimelineWidget.h
	ChannelWidget.h
	ChannelCheckbox.h
	${HEADER_PATH}/TimeSliderWidget.h
	${HEADER_PATH}/TimelineControlsWidget.h
)
QT4_WRAP_CPP(TARGET_MOC_SRC ${TARGET_MOC_H})
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_MOC_SRC})
set(TARGET_SRC ${TARGET_SRC} ${TARGET_MOC_SRC})

#------------------------------------------------------------------------------
# UI
SET(TARGET_UI
	ui/TimelineWidget.ui
	ui/TimelineControlsWidget.ui
)
QT4_WRAP_UI(TARGET_UI_H ${TARGET_UI})
source_group("${SOURCEGROUP_UI}" FILES ${TARGET_UI_H} ${TARGET_UI})
set(TARGET_H ${TARGET_H} ${TARGET_UI} ${TARGET_UI_H})

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
#instalacja
install(FILES ${PUBLIC_H} DESTINATION include/plugins/${PROJECT_NAME} COMPONENT ${PROJECT_NAME}_dev)
if (WIN32)
install(TARGETS ${TARGET_TARGETNAME} RUNTIME DESTINATION bin COMPONENT ${PROJECT_NAME})
endif()
install(TARGETS ${TARGET_TARGETNAME} LIBRARY DESTINATION lib/plugins/${PROJECT_NAME} ARCHIVE DESTINATION lib/plugins/${PROJECT_NAME} COMPONENT ${PROJECT_NAME}_dev)
# nag³ówki
include_directories( ${DEFAULT_PROJECT_INCLUDES} )
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	

# ¿eby mo¿na by³o u¿yæ tej biblioteki w makrach
ON_PROJECT_ADDED(${TARGET_TARGETNAME})
