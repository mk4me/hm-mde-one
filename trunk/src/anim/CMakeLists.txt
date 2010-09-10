PROJECT(ANIMSERV_TEST)

# just debug / release
SET(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ${ANIMSERV_TEST_SOURCE_DIR} ${ANIMSERV_TEST_BINARY_DIR})

ADD_DEFINITIONS(-D__dll__)

# add Qt modules here, the include will setup QT_LIBRARIES
SET(QT_USE_QTOPENGL TRUE) # obsluga OpenGL
INCLUDE(${QT_USE_FILE})

# files which need to be moc'd by Qt
SET(ANIMSERV_TEST_SRCS
	AnimationPlugin.h
	OsgControlWidget.h
)

# UI
SET(ANIMSERV_TEST_UI
	UI/OsgWidget.ui
)

IF(UNIX)

	# .cxx sources
	SET(ANIMSERV_TEST_SRCS_CXX
		AnimationPlugin.cpp
		OsgControlWidget.cpp
		QtWidget.cpp
		PluginData.cpp
		AnimationService.cpp
		Animation.cpp
	)

	# INCLUDES
	SET(ANIMSERV_TEST_INCLUDE_X
		../../include/osg
		../include
		UI
		/usr/X11R6/include
		../osgext/
	)

	# LIBS
	SET(ANIMSERV_TEST_LIBS_X
		dl
		m
		GLU
		GL
		pthread
		osg
		osgViewer
		osgSim
		osgUtil	
	)

ELSEIF(WIN32)

	# .cxx sources
	SET(ANIMSERV_TEST_SRCS_CXX
		AnimationPlugin.cpp
		OsgControlWidget.cpp
		QtWidget.cpp
		PluginData.cpp
		AnimationService.cpp
		Animation.cpp

		AnimationPlugin.h
		OsgControlWidget.h
		QtWidget.h
		PluginData.h
		AnimationService.h
		Animation.h
		../include/IServiceManager.h
		../include/ISkeletonNode.h
		../include/IAnimationGroup.h
		../include/IModel.h
		../include/IMesh.h
	)

	# INCLUDES
	SET(ANIMSERV_TEST_INCLUDE_X
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
	SET(ANIMSERV_TEST_LIBS_X
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

INCLUDE_DIRECTORIES(${ANIMSERV_TEST_INCLUDE_X})

# build ui_XXX files from the XML-style .ui files
QT4_WRAP_UI(ANIMSERV_TEST_SRCS_CXX ${ANIMSERV_TEST_UI})
# this moc's the above variable and appends to the cxx sources
QT4_WRAP_CPP(ANIMSERV_TEST_SRCS_CXX ${ANIMSERV_TEST_SRCS})

# srcs
IF(UNIX)
  ADD_LIBRARY(AnimServPlugin_test SHARED ${ANIMSERV_TEST_SRCS_CXX})
ELSEIF(WIN32)
  ADD_LIBRARY(AnimServPlugin_test SHARED ${ANIMSERV_TEST_SRCS_CXX})
ENDIF()

# libs
TARGET_LINK_LIBRARIES(AnimServPlugin_test ${QT_LIBRARIES})
TARGET_LINK_LIBRARIES(AnimServPlugin_test ${ANIMSERV_TEST_LIBS_X})

IF(WIN32)
	#prefix hack
	SET_TARGET_PROPERTIES(AnimServPlugin_test PROPERTIES
		PREFIX ../../../bin/Win32/$(ConfigurationName)/plugins/
	)
ELSEIF(UNIX)
	# post build event
	ADD_CUSTOM_COMMAND(
		TARGET ControlPlugin
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ARGS AnimServPlugin_test.so ../../bin/Linux/plugins/AnimServPlugin_test.so
	)
ENDIF()
