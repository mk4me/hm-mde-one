PROJECT(TOOLBOX)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ${TOOLBOX_SOURCE_DIR} ${TOOLBOX_BINARY_DIR})

# add Qt modules here, the include will setup QT_LIBRARIES
SET(QT_USE_QTOPENGL TRUE) # obsluga OpenGL
INCLUDE(${QT_USE_FILE})

# files which need to be moc'd by Qt
SET(TOOLBOX_SRCS
    ../../Toolbox/ConsoleWidget.h
    ../../Toolbox/OsgControlWidget.h
    ../../Toolbox/TimeLine.h
    ../../Toolbox/toolboxmain.h
	../../Toolbox/GridWidget.h
)

# UI
SET(TOOLBOX_UI
	../../Toolbox/UI/Console.ui
    ../../Toolbox/UI/OsgWidget.ui
	../../Toolbox/UI/TimeLine.ui
    ../../Toolbox/UI/toolboxmaindeffile.ui
    ../../Toolbox/UI/GridWidget.ui
)

IF(UNIX)

	# .cxx sources
	SET(TOOLBOX_SRCS_CXX
		../../Toolbox/AdapterWidget.cpp
		../../Toolbox/AnimationService.cpp
		../../Toolbox/ConfigurationService.cpp
		../../Toolbox/ObjectService.cpp
		../../Toolbox/ConsoleWidget.cpp
		../../Toolbox/Logger.cpp
		../../Toolbox/main.cpp
		../../Toolbox/Model.cpp
		../../Toolbox/ModelWithSkeleton.cpp
		../../Toolbox/OsgControlWidget.cpp
		../../Toolbox/GridWidget.cpp
		../../Toolbox/QOSGWidget.cpp
		../../Toolbox/ServiceManager.cpp
		../../Toolbox/TimeLine.cpp
		../../Toolbox/toolboxmain.cpp
		../../Toolbox/Animation.cpp
		../../Toolbox/ModelFacade.cpp
	)

	# INCLUDES
	SET(TOOLBOX_INCLUDE_X
		../../include
		../../Toolbox/UI
		/usr/X11R6/include
		../../OsgExtensions/
	)

	# LIBS
	SET(TOOLBOX_LIBS_X
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
	SET(TOOLBOX_SRCS_CXX
		../../Toolbox/AdapterWidget.cpp
		../../Toolbox/AnimationService.cpp
		../../Toolbox/ObjectService.cpp
		../../Toolbox/ConfigurationService.cpp
		../../Toolbox/ConsoleWidget.cpp
		../../Toolbox/Logger.cpp
		../../Toolbox/main.cpp
		../../Toolbox/Model.cpp
		../../Toolbox/ModelWithSkeleton.cpp
		../../Toolbox/OsgControlWidget.cpp
		../../Toolbox/GridWidget.cpp	
		../../Toolbox/QOSGWidget.cpp
		../../Toolbox/ServiceManager.cpp
		../../Toolbox/TimeLine.cpp
		../../Toolbox/toolboxmain.cpp
		../../Toolbox/Animation.cpp
		../../Toolbox/GridWidget.cpp	
		../../Toolbox/ModelFacade.cpp
		../../Toolbox/AdapterWidget.h
		../../Toolbox/AnimationService.h
		../../Toolbox/ConfigurationService.h
		../../Toolbox/ObjectService.h
		../../Toolbox/ConsoleWidget.h
		../../Toolbox/Logger.h
		../../Toolbox/Model.h
		../../Toolbox/ModelWithSkeleton.h
		../../Toolbox/OsgControlWidget.h
		../../Toolbox/GridWidget.h
		../../Toolbox/QOSGWidget.h
		../../Toolbox/ServiceManager.h
		../../Toolbox/TimeLine.h
		../../Toolbox/toolboxmain.h
		../../Toolbox/Animation.h
		../../Toolbox/SimpleFunctors.h
		
#		../../ObjectService.inl
#		../../toolboxmain.inl
		
		../../Toolbox/BaseDataTypes.h
		../../Toolbox/IBaseService.h
		../../Toolbox/GlobalServicesIDs.h
		../../Toolbox/ModelFacade.h
	)

	# INCLUDES
	SET(TOOLBOX_INCLUDE_X
		$(QTDIR)/include/QtCore
		$(QTDIR)/include/QtGui
		$(QTDIR)/include
		$(QTDIR)/include/ActiveQt
		debug
		$(QTDIR)/mkspecs/win32-msvc2008
		../../include
		../../../../Tools/lua/include
		../../OsgExtensions/
	)

	# LIBS
	SET(TOOLBOX_LIBS_X
		$(QTDIR)/lib/qtmaind.lib 
		$(QTDIR)/lib/QtOpenGLd4.lib 
		$(QTDIR)/lib/QtGuid4.lib 
		$(QTDIR)/lib/QtCored4.lib 
		osgd.lib 
		osgViewerd.lib 
		osgGAd.lib 
		osgDBd.lib 
		osgTextd.lib 
		OpenThreadsd.lib 
		../../../../Tools/lua/lib/static/lua5.1
		../OsgExtensions/osgext
	)

ENDIF()

INCLUDE_DIRECTORIES(${TOOLBOX_INCLUDE_X})

# build ui_XXX files from the XML-style .ui files
QT4_WRAP_UI(TOOLBOX_SRCS_CXX ${TOOLBOX_UI})
# this moc's the above variable and appends to the cxx sources
QT4_WRAP_CPP(TOOLBOX_SRCS_CXX ${TOOLBOX_SRCS})

# srcs
IF(UNIX)
  ADD_EXECUTABLE(toolbox_app ${TOOLBOX_SRCS_CXX})
ELSEIF(WIN32)
  ADD_EXECUTABLE(toolbox_app WIN32 ${TOOLBOX_SRCS_CXX})
ENDIF()

# libs
TARGET_LINK_LIBRARIES(toolbox_app ${QT_LIBRARIES})
TARGET_LINK_LIBRARIES(toolbox_app ${TOOLBOX_LIBS_X})

IF(WIN32)
	# post build event
	ADD_CUSTOM_COMMAND(
		TARGET toolbox_app
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E comspec ARGS Promote.cmd $(ConfigurationName) Win32
		COMMAND ${CMAKE_COMMAND} -E copy ARGS $(ConfigurationName)/toolbox_app.exe ../../../../bin/Win32/$(ConfigurationName)/
	)
ELSEIF(UNIX)
	# post build event
	ADD_CUSTOM_COMMAND(
		TARGET toolbox_app
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ARGS toolbox_app ../../../../bin/Linux/toolbox_app
	)
ENDIF()

