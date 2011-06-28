set(TARGET_TARGETNAME
	newTimelineSample
)

#------------------------------------------------------------------------------
# nag³ówki
set(TARGET_H
	ChannelsTree.h
	ChannelsTreeItem.h
	SampleWindow.h
	TimeChannelWidget.h
	TestClientChannel.h
	TimelineWidget.h
	TimeCounter.h
	ChannelsTreeHeaderWidget.h
)
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${TARGET_H})

#------------------------------------------------------------------------------
# Ÿród³a
set(TARGET_SRC 
	ChannelsTree.cpp
	ChannelsTreeItem.cpp
	QtTimelineSample.cpp
	SampleWindow.cpp
	TimeChannelWidget.cpp
	TestClientChannel.cpp
	TimelineWidget.cpp
	TimeCounter.cpp
	ChannelsTreeHeaderWidget.cpp
)
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_SRC})

#------------------------------------------------------------------------------
# metadane generowane przez MOC
set(TARGET_MOC_H
    SampleWindow.h
	ChannelsTree.h
	TimeChannelWidget.h
	TimelineWidget.h
	ChannelsTreeHeaderWidget.h
)
QT4_WRAP_CPP(TARGET_MOC_SRC ${TARGET_MOC_H})
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_MOC_SRC})
set(TARGET_SRC ${TARGET_SRC} ${TARGET_MOC_SRC})

#------------------------------------------------------------------------------
# metadane generowane przez UI
set(TARGET_UI
	SampleWindow.ui
	TimelineWidget.ui
)
QT4_WRAP_UI(TARGET_UI_H ${TARGET_UI})
source_group("${SOURCEGROUP_UI}" FILES ${TARGET_UI_H} ${TARGET_UI})
set(TARGET_H ${TARGET_H} ${TARGET_UI_H} ${TARGET_UI})

#------------------------------------------------------------------------------
# RC
#set(TARGET_RC
#	ui/ConsoleWidget.qrc
#)
#QT4_ADD_RESOURCES(TARGET_RCC_SRC ${TARGET_RC})
#source_group("${SOURCEGROUP_UI}" FILES ${TARGET_RC} ${TARGET_RCC_SRC})
#set(TARGET_SRC ${TARGET_SRC} ${TARGET_RCC_SRC} ${TARGET_RC})

# obsluga OpenGL
set(QT_USE_QTOPENGL TRUE) 
include(${QT_USE_FILE})

# kompilacja do wspó³dzielonej biblioteki
add_executable(${TARGET_TARGETNAME} ${TARGET_SRC} ${TARGET_H})
# nag³ówki
include_directories( ${DEFAULT_PROJECT_INCLUDES} )
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	
# labelka
set_target_properties(${TARGET_TARGETNAME} PROPERTIES PROJECT_LABEL "Example ${TARGET_TARGETNAME}")