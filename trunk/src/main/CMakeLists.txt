PROJECT(TOOLBOX)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ${TOOLBOX_SOURCE_DIR} ${TOOLBOX_BINARY_DIR})

# add Qt modules here, the include will setup QT_LIBRARIES
SET(QT_USE_QTOPENGL TRUE) # obsluga OpenGL
INCLUDE(${QT_USE_FILE})

# files which need to be moc'd by Qt
SET(TOOLBOX_SRCS
    ConsoleWidget.h
    OsgControlWidget.h
    TimeLine.h
    ToolboxMain.h
	GridWidget.h
)

# UI
SET(TOOLBOX_UI
	UI/Console.ui
    UI/OsgWidget.ui
	UI/TimeLine.ui
    UI/toolboxmaindeffile.ui
    UI/GridWidget.ui
)

IF(UNIX)

	# .cxx sources
	SET(TOOLBOX_SRCS_CXX
		AdapterWidget.cpp
		AnimationService.cpp
		ObjectService.cpp
		ConsoleWidget.cpp
		Logger.cpp
		main.cpp
		Model.cpp
		Mesh.cpp
		ModelWithSkeleton.cpp
		OsgControlWidget.cpp
		GridWidget.cpp
		QOSGWidget.cpp
		ServiceManager.cpp
		TimeLine.cpp
		ToolboxMain.cpp
		Animation.cpp
		ModelFacade.cpp
		../utils/ConfigFileManage/ConfigBuffer.cpp
		../utils/ConfigFileManage/ConfigurationFile.cpp
		../utils/ConfigFileManage/ConfigurationGroup.cpp
		../utils/ConfigFileManage/FileReader.cpp
		../utils/ConfigFileManage/Parameter.cpp
	)

	# INCLUDES
	SET(TOOLBOX_INCLUDE_X
		../include
		UI
		/usr/X11R6/include
		../OsgExtensions/
		../utils/ConfigFileManage
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
		osgext
	)

ELSEIF(WIN32)

	# .cxx sources
	SET(TOOLBOX_SRCS_CXX
		AdapterWidget.cpp
		AnimationService.cpp
		ObjectService.cpp
		ConsoleWidget.cpp
		Logger.cpp
		main.cpp
		Model.cpp
		Mesh.cpp
		OsgControlWidget.cpp
		GridWidget.cpp	
		QOSGWidget.cpp
		ServiceManager.cpp
		TimeLine.cpp
		ToolboxMain.cpp
		Animation.cpp
		GridWidget.cpp	
		ModelFacade.cpp
		../utils/ConfigFileManage/ConfigBuffer.cpp
		../utils/ConfigFileManage/ConfigurationFile.cpp
		../utils/ConfigFileManage/ConfigurationGroup.cpp
		../utils/ConfigFileManage/FileReader.cpp
		../utils/ConfigFileManage/Parameter.cpp
		
		
		AdapterWidget.h
		AnimationService.h
		ObjectService.h
		ConsoleWidget.h
		Logger.h
		Model.h
		Mesh.h
		OsgControlWidget.h
		GridWidget.h
		QOSGWidget.h
		ServiceManager.h
		TimeLine.h
		ToolboxMain.h
		Animation.h
		SimpleFunctors.h	
		BaseDataTypes.h
		IBaseService.h
		GlobalServicesIDs.h
		ModelFacade.h
		../utils/ConfigFileManage/ConfigBuffer.h
		../utils/ConfigFileManage/ConfigurationFile.h
		../utils/ConfigFileManage/ConfigurationGroup.h
		../utils/ConfigFileManage/FileReader.h
		../utils/ConfigFileManage/Parameter.h
		../utils/ConfigFileManage/IConfigManager.h
	)

	# INCLUDES
	SET(TOOLBOX_INCLUDE_X
		../../include/QT/QtCore
		../../include/QT/QtGui
		../../include/QT/
		../../include/QT/ActiveQt
		debug
		../../include/mkspecs/win32-msvc2008
		../include
		../OsgExtensions/
	)

	# LIBS
	SET(TOOLBOX_LIBS_X
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
		../lib/osgext
	)
	
	# Tworzymy grupy (filtry VS)
	# ---------------------------------------------------------------
	#
	#SOURCE_GROUP("Header Files\\ConfigFileManage" FILES 
	
	SOURCE_GROUP("Header Files\\Toolbox" FILES  
		AdapterWidget.h
		AnimationService.h
		ObjectService.h
		ConsoleWidget.h
		Logger.h
		Model.h
		Mesh.h
		OsgControlWidget.h
		GridWidget.h
		QOSGWidget.h
		ServiceManager.h
		TimeLine.h
		ToolboxMain.h
		Animation.h
		SimpleFunctors.h	
		BaseDataTypes.h
		IBaseService.h
		GlobalServicesIDs.h
		ModelFacade.h
		)
		
	SOURCE_GROUP("Source Files\\Toolbox" FILES  
		AdapterWidget.cpp
		AnimationService.cpp
		ObjectService.cpp
		ConsoleWidget.cpp
		Logger.cpp
		main.cpp
		Model.cpp
		Mesh.cpp
		OsgControlWidget.cpp
		GridWidget.cpp	
		QOSGWidget.cpp
		ServiceManager.cpp
		TimeLine.cpp
		ToolboxMain.cpp
		Animation.cpp
		GridWidget.cpp	
		ModelFacade.cpp
		)
		
	
	SOURCE_GROUP("Header Files\\ConfigFileManage" FILES  
		../utils/ConfigFileManage/ConfigBuffer.h
		../utils/ConfigFileManage/ConfigurationFile.h
		../utils/ConfigFileManage/ConfigurationGroup.h
		../utils/ConfigFileManage/FileReader.h
		../utils/ConfigFileManage/Parameter.h
		../utils/ConfigFileManage/IConfigManager.h
		)
		
	SOURCE_GROUP("Source Files\\ConfigFileManage" FILES  
		../utils/ConfigFileManage/ConfigBuffer.cpp
		../utils/ConfigFileManage/ConfigurationFile.cpp
		../utils/ConfigFileManage/ConfigurationGroup.cpp
		../utils/ConfigFileManage/FileReader.cpp
		../utils/ConfigFileManage/Parameter.cpp
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
		COMMAND ${CMAKE_COMMAND} -E copy ARGS $(ConfigurationName)/toolbox_app.exe ../../bin/Win32/$(ConfigurationName)/
	)
ELSEIF(UNIX)
	# post build event
	ADD_CUSTOM_COMMAND(
		TARGET toolbox_app
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ARGS toolbox_app ../../bin/Linux/toolbox_app
	)
ENDIF()

