PROJECT(TOOLBOX)

# just debug / release
SET(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

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
		PluginServices.cpp
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
		FileUtil.cpp
		
		../utils/ConfigFileManage/ConfigBuffer.cpp
		../utils/ConfigFileManage/ConfigurationGroup.cpp
		../utils/ConfigFileManage/FileReader.cpp
		../utils/ConfigFileManage/Parameter.cpp
		../utils/ConfigFileManage/ConfigurationFileService.cpp
		
		../OsgExtensions/CustomGroup.cpp
		../OsgExtensions/SkeletonNode.cpp
		../OsgExtensions/AnimationNode.cpp
		../OsgExtensions/AnimationGroup.cpp	
	)

	# INCLUDES
	SET(TOOLBOX_INCLUDE_X
		../include
		UI
		/usr/X11R6/include
		../osgext/
		../utils/ConfigFileManage
		../../include/osg
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
		PluginServices.cpp
		FileUtil.cpp
		../utils/ConfigFileManage/ConfigBuffer.cpp
		../utils/ConfigFileManage/ConfigurationGroup.cpp
		../utils/ConfigFileManage/FileReader.cpp
		../utils/ConfigFileManage/Parameter.cpp
		../utils/ConfigFileManage/ConfigurationFileService.cpp
		
		../OsgExtensions/CustomGroup.cpp
		../OsgExtensions/SkeletonNode.cpp
		../OsgExtensions/AnimationNode.cpp
		../OsgExtensions/AnimationGroup.cpp	
		
		
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
		PluginServices.h
		FileUtil.h
		../utils/ConfigFileManage/ConfigBuffer.h
		../utils/ConfigFileManage/ConfigurationGroup.h
		../utils/ConfigFileManage/FileReader.h
		../utils/ConfigFileManage/Parameter.h
		../utils/ConfigFileManage/IConfigManager.h
		../utils/ConfigFileManage/ConfigurationFileService.h
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
		../../include/osg
	)

	# LIBS
	SET(TOOLBOX_LIBS_X
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
	
	SET(MANAGER_DLLS
		osg55-osgDB.dll
	)
	
	# Tworzymy grupy (filtry VS)
	# ---------------------------------------------------------------
	#
	#SOURCE_GROUP("Header Files\\ConfigFileManage" FILES 
	
	SOURCE_GROUP("Header Files\\Toolbox" FILES  
		AdapterWidget.h
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
		FileUtil.h
		)
		
	SOURCE_GROUP("Source Files\\Toolbox" FILES  
		AdapterWidget.cpp
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
		FileUtil.cpp
		../OsgExtensions/CustomGroup.cpp
		../OsgExtensions/SkeletonNode.cpp
		../OsgExtensions/AnimationNode.cpp
		../OsgExtensions/AnimationGroup.cpp	
		)
		
	
	SOURCE_GROUP("Header Files\\ConfigFileManage" FILES  
		../utils/ConfigFileManage/ConfigBuffer.h
		../utils/ConfigFileManage/ConfigurationGroup.h
		../utils/ConfigFileManage/FileReader.h
		../utils/ConfigFileManage/Parameter.h
		../utils/ConfigFileManage/IConfigManager.h
		)
		
	SOURCE_GROUP("Source Files\\ConfigFileManage" FILES  
		../utils/ConfigFileManage/ConfigBuffer.cpp
		../utils/ConfigFileManage/ConfigurationGroup.cpp
		../utils/ConfigFileManage/FileReader.cpp
		../utils/ConfigFileManage/Parameter.cpp
		)
		
	SOURCE_GROUP("Header Files\\Services" FILES  
		../utils/ConfigFileManage/ConfigurationFileService.h
		AnimationService.h
		ObjectService.h
		PluginServices.h
	)
	
	SOURCE_GROUP("Source Files\\Services" FILES  
		../utils/ConfigFileManage/ConfigurationFileService.cpp
		AnimationService.cpp
		ObjectService.cpp
		PluginServices.cpp
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
  ADD_EXECUTABLE(toolbox_app  ${TOOLBOX_SRCS_CXX})
ENDIF()

# libs
TARGET_LINK_LIBRARIES(toolbox_app ${QT_LIBRARIES})
TARGET_LINK_LIBRARIES(toolbox_app ${TOOLBOX_LIBS_X})

install(TARGETS toolbox_app DESTINATION bin )
IF(WIN32)
	# post build event
	#ADD_CUSTOM_COMMAND(
	#	TARGET toolbox_app
	#	POST_BUILD
	#	COMMAND ${CMAKE_COMMAND} -E comspec ARGS Promote.cmd $(ConfigurationName) Win32
		#COMMAND ${CMAKE_COMMAND} -E copy ARGS $(ConfigurationName)/toolbox_app.exe ../../bin/Win32/$(ConfigurationName)/
	#)
	
	#prefix hack
	#SET_TARGET_PROPERTIES(toolbox_app PROPERTIES
	#	PREFIX ../../../bin/Win32/$(ConfigurationName)/
	#	DEBUG_POSTFIX _d
	#)

	set(DLL_FILES ../../lib/win32/release/osg/osg66-osg.dll)
	set(DLL_FILES ${DLL_FILES} ../../lib/win32/release/osg/osg66-osgDB.dll)
	set(DLL_FILES ${DLL_FILES} ../../lib/win32/release/osg/osg66-osgGA.dll)
	set(DLL_FILES ${DLL_FILES} ../../lib/win32/release/osg/osg66-osgQT.dll)
	set(DLL_FILES ${DLL_FILES} ../../lib/win32/release/osg/osg66-osgText.dll)
	set(DLL_FILES ${DLL_FILES} ../../lib/win32/release/osg/osg66-osgUtil.dll)
	set(DLL_FILES ${DLL_FILES} ../../lib/win32/release/osg/osg66-osgViewer.dll)
	set(DLL_FILES ${DLL_FILES} ../../lib/win32/release/osg/osg66-osgWidget.dll)
	set(DLL_FILES ${DLL_FILES} ../../lib/win32/release/osg/ot12-OpenThreads.dll)
	set(DLL_FILES ${DLL_FILES} ../../lib/win32/release/QT/QtGui4.dll)
	set(DLL_FILES ${DLL_FILES} ../../lib/win32/release/QT/QtCore4.dll)
	set(DLL_FILES ${DLL_FILES} ../../lib/win32/release/QT/QtOpenGL4.dll)

	install(FILES ${DLL_FILES} DESTINATION bin)
ELSEIF(UNIX)
	# post build event
	ADD_CUSTOM_COMMAND(
		TARGET toolbox_app
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ARGS toolbox_app ../../bin/Linux/toolbox_app
	)
ENDIF()

