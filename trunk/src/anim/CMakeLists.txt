project(ANIMSERV_TEST)
set(TARGET_TARGETNAME anim)

set(TARGET_H
	AnimationPlugin.h
	OsgControlWidget.h
	QtWidget.h
	PluginData.h
	AnimationService.h
	Animation.h
)

set(TARGET_SRC
	AnimationPlugin.cpp
	OsgControlWidget.cpp
	QtWidget.cpp
	PluginData.cpp
	AnimationService.cpp
	Animation.cpp
)


ADD_DEFINITIONS(-D__dll__)


# add Qt modules here, the include will setup QT_LIBRARIES
set(QT_USE_QTOPENGL TRUE) # obsluga OpenGL
include(${QT_USE_FILE})

# files which need to be moc'd by Qt
SET(TARGET_MOC_H
	AnimationPlugin.h
	OsgControlWidget.h
)
QT4_WRAP_CPP(TARGET_SRC ${TARGET_MOC_H})

# UI
SET(TARGET_UI_H
	UI/OsgWidget.ui
)
QT4_WRAP_UI(TARGET_H ${TARGET_UI_H})

# kompilacja do wspó³dzielonej biblioteki
add_library(${TARGET_TARGETNAME} STATIC ${TARGET_SRC} ${TARGET_H})
# nag³ówki
include_directories( ${DEFAULT_PROJECT_INCLUDES} )
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	

# ¿eby mo¿na by³o u¿yæ tej biblioteki w makrach
ON_PROJECT_ADDED(${TARGET_TARGETNAME})

IF(WIN32)
	#prefix hack
	SET_TARGET_PROPERTIES(anim PROPERTIES
		PREFIX ../../../bin/Win32/$(ConfigurationName)/plugins/
	)
ELSEIF(UNIX)
	# post build event
	ADD_CUSTOM_COMMAND(
		TARGET ControlPlugin
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ARGS anim.so ../../bin/Linux/plugins/anim.so
	)
ENDIF()
