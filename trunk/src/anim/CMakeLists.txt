PROJECT(CONTROLPLUGIN)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ${CONTROLPLUGIN_SOURCE_DIR} ${CONTROLPLUGIN_BINARY_DIR})

# add Qt modules here, the include will setup QT_LIBRARIES
SET(QT_USE_QTOPENGL TRUE) # obsluga OpenGL
INCLUDE(${QT_USE_FILE})

# files which need to be moc'd by Qt
SET(CONTROLPLUGIN_SRCS
	ControlPlugin.h
	OsgControlWidget.h
)

# UI
SET(CONTROLPLUGIN_UI
	UI/OsgWidget.ui
)

IF(UNIX)

	# .cxx sources
	SET(CONTROLPLUGIN_SRCS_CXX
		ControlPlugin.cpp
		OsgControlWidget.cpp
	)

	# INCLUDES
	SET(CONTROLPLUGIN_INCLUDE_X
		../include
		UI
		/usr/X11R6/include
		../OsgExtensions/
	)

	# LIBS
	SET(CONTROLPLUGIN_LIBS_X
		dl
		m
		GLU
		GL
		pthread
		osg
		osgViewer
		osgSim
		osgUtil
		lua
		osgext		
	)

ELSEIF(WIN32)

	# .cxx sources
	SET(CONTROLPLUGIN_SRCS_CXX
		ControlPlugin.cpp
		OsgControlWidget.cpp

		ControlPlugin.h
		OsgControlWidget.h
	)

	# INCLUDES
	SET(CONTROLPLUGIN_INCLUDE_X
		../../include
		../../include/QT/QtCore
		../../include/QT/QtGui
		../../include/QT
		../../include/QT/ActiveQt
		debug
		../../include/mkspecs/win32-msvc2008
		../include
		../utils/Tools/lua/include
		../OsgExtensions/
	)

	# LIBS
	SET(CONTROLPLUGIN_LIBS_X
		../../lib/win32/debug/QT/qtmaind
		../../lib/win32/debug/QT/QtOpenGLd4
		../../lib/win32/debug/QT/QtGuid4
		../../lib/win32/debug/QT/QtCored4
		osgd.lib 
		osgViewerd.lib 
		osgGAd.lib 
		osgDBd.lib 
		osgTextd.lib 
		OpenThreadsd.lib 
		../utils/Tools/lua/lib/static/lua5.1
		../lib/osgext		
	)

ENDIF()

INCLUDE_DIRECTORIES(${CONTROLPLUGIN_INCLUDE_X})

# build ui_XXX files from the XML-style .ui files
QT4_WRAP_UI(CONTROLPLUGIN_SRCS_CXX ${CONTROLPLUGIN_UI})
# this moc's the above variable and appends to the cxx sources
QT4_WRAP_CPP(CONTROLPLUGIN_SRCS_CXX ${CONTROLPLUGIN_SRCS})

# srcs
IF(UNIX)
  ADD_LIBRARY(ControlPlugin SHARED ${CONTROLPLUGIN_SRCS_CXX})
ELSEIF(WIN32)
  ADD_LIBRARY(ControlPlugin SHARED ${CONTROLPLUGIN_SRCS_CXX})
ENDIF()

# libs
TARGET_LINK_LIBRARIES(ControlPlugin ${QT_LIBRARIES})
TARGET_LINK_LIBRARIES(ControlPlugin ${CONTROLPLUGIN_LIBS_X})

IF(WIN32)
	# post build event
	ADD_CUSTOM_COMMAND(
		TARGET ControlPlugin
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory ../../bin
		COMMAND ${CMAKE_COMMAND} -E make_directory ../../bin/Win32
		COMMAND ${CMAKE_COMMAND} -E make_directory ../../bin/Win32/$(ConfigurationName)
		COMMAND ${CMAKE_COMMAND} -E make_directory ../../bin/Win32/$(ConfigurationName)/plugins
		COMMAND ${CMAKE_COMMAND} -E make_directory ../main/$(ConfigurationName)/plugins
		COMMAND ${CMAKE_COMMAND} -E copy ARGS $(ConfigurationName)/ControlPlugin.dll ../../bin/Win32/$(ConfigurationName)/plugins/
		COMMAND ${CMAKE_COMMAND} -E copy ARGS $(ConfigurationName)/ControlPlugin.dll ../main/$(ConfigurationName)/plugins/
	)
ELSEIF(UNIX)
	# post build event
	ADD_CUSTOM_COMMAND(
		TARGET ControlPlugin
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ARGS libControlPlugin.so ../../bin/Linux/plugins/ControlPlugin.so
	)
ENDIF()
