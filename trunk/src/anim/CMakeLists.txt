PROJECT(CONTROLPLUGIN)

# just debug / release
SET(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

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
		../../include/osg
		../include
		UI
		/usr/X11R6/include
		../osgext/
	)

	# LIBS
	SET(CONTROLPLUGIN_LIBS_X
		dl
		m
		GLU
		GL
		pthread
		${PROJECT_SOURCE_DIR}/../../lib/linux/release/osg/libosg.so
		${PROJECT_SOURCE_DIR}/../../lib/linux/release/osg/libosgViewer.so
		${PROJECT_SOURCE_DIR}/../../lib/linux/release/osg/libosgSim.so
		${PROJECT_SOURCE_DIR}/../../lib/linux/release/osg/libosgUtil.so	
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
		../../include/osg
		../../include
		../../include/QT/QtCore
		../../include/QT/QtGui
		../../include/QT
		../../include/QT/ActiveQt
		debug
		../../include/mkspecs/win32-msvc2008
		../include
		../osgext/
	)

	# LIBS
	SET(CONTROLPLUGIN_LIBS_X
		debug ../../lib/win32/debug/QT/qtmaind
		debug ../../lib/win32/debug/QT/QtOpenGLd4
		debug ../../lib/win32/debug/QT/QtGuid4
		debug ../../lib/win32/debug/QT/QtCored4
		debug ../../lib/win32/debug/osg/osgd
		debug ../../lib/win32/debug/osg/osgViewerd
		debug ../../lib/win32/debug/osg/osgGAd
		debug ../../lib/win32/debug/osg/osgDBd
		debug ../../lib/win32/debug/osg/osgTextd
		debug ../../lib/win32/debug/osg/OpenThreadsd
			
		
		optimized ../../lib/win32/release/QT/qtmain
		optimized ../../lib/win32/release/QT/QtOpenGL4
		optimized ../../lib/win32/release/QT/QtGui4
		optimized ../../lib/win32/release/QT/QtCore4
		optimized ../../lib/win32/release/osg/osg
		optimized ../../lib/win32/release/osg/osgViewer
		optimized ../../lib/win32/release/osg/osgGA
		optimized ../../lib/win32/release/osg/osgDB
		optimized ../../lib/win32/release/osg/osgText
		optimized ../../lib/win32/release/osg/OpenThreads
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

install(TARGETS ControlPlugin DESTINATION bin/plugins)
IF(WIN32)
	# post build event
	#ADD_CUSTOM_COMMAND(
	#	TARGET ControlPlugin
	#	POST_BUILD
	#	COMMAND ${CMAKE_COMMAND} -E make_directory ../../bin
	#	COMMAND ${CMAKE_COMMAND} -E make_directory ../../bin/Win32
	#	COMMAND ${CMAKE_COMMAND} -E make_directory ../../bin/Win32/$(ConfigurationName)
	#	COMMAND ${CMAKE_COMMAND} -E make_directory ../../bin/Win32/$(ConfigurationName)/plugins
	#	COMMAND ${CMAKE_COMMAND} -E copy ARGS $(ConfigurationName)/ControlPlugin.dll ../../bin/Win32/$(ConfigurationName)/plugins/
	#)

ELSEIF(UNIX)
	# post build event
#	ADD_CUSTOM_COMMAND(
##		TARGET ControlPlugin
#		POST_BUILD
#		COMMAND ${CMAKE_COMMAND} -E copy ARGS libControlPlugin.so ../../bin/Linux/plugins/ControlPlugin.so
#	)
ENDIF()
