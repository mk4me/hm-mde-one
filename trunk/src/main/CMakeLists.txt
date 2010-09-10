PROJECT(TOOLBOX)

# just debug / release
SET(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ${TOOLBOX_SOURCE_DIR} ${TOOLBOX_BINARY_DIR})

# add Qt modules here, the include will setup QT_LIBRARIES
SET(QT_USE_QTOPENGL TRUE) # obsluga OpenGL
INCLUDE(${QT_USE_FILE})

macro(copy_files GLOBPAT DESTINATION)
	# Find all the input files
	FILE(GLOB copy_file RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "${GLOBPAT}/*.dll")
    #FILE(APPEND CMAKELOGER.txt copy_file: ${copy_file} \n)
    
    #FILE(APPEND CMAKELOGER.txt Cmake CURRENT Source Dir: ${CMAKE_CURRENT_SOURCE_DIR} \n)
    #FILE(APPEND CMAKELOGER.txt CURRENT Directories: ${GLOBPAT} \n)
    add_custom_target(copy ALL
    COMMENT "Copying files: ${GLOBPAT}")

  foreach(FILENAME ${copy_file})
	#FILE(APPEND CMAKELOGER.txt FILENAME: ${FILENAME} \n)
    set(SRC "${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}")
    set(DST "${CMAKE_CURRENT_SOURCE_DIR}/${DESTINATION}")

    #FILE(APPEND CMAKELOGER.txt SRC: ${SRC} \n)
    #FILE(APPEND CMAKELOGER.txt DST: ${DST} \n)

    add_custom_command(
      TARGET copy
	  COMMAND ${CMAKE_COMMAND} -E make_directory ../../bin
	  COMMAND ${CMAKE_COMMAND} -E make_directory ../../bin/Win32
	  COMMAND ${CMAKE_COMMAND} -E make_directory ../../bin/Win32/$(ConfigurationName)
	  COMMAND ${CMAKE_COMMAND} -E make_directory ../../bin/Win32/$(ConfigurationName)/osgPlugins-2.8.2/
      COMMAND ${CMAKE_COMMAND} -E copy ARGS ${SRC} ${DST}
      )
  endforeach(FILENAME)
endmacro(copy_files)


# files which need to be moc'd by Qt
SET(TOOLBOX_SRCS
    ConsoleWidget.h
    TimeLine.h
    ToolboxMain.h
	GridWidget.h
)

# UI
SET(TOOLBOX_UI
	UI/Console.ui
	UI/TimeLine.ui
    UI/toolboxmaindeffile.ui
    UI/GridWidget.ui
)

IF(UNIX)

	# .cxx sources
	SET(TOOLBOX_SRCS_CXX
		AdapterWidget.cpp
		ConsoleWidget.cpp
		Logger.cpp
		main.cpp
		GridWidget.cpp	
		QOSGWidget.cpp
		ServiceManager.cpp
		TimeLine.cpp
		ToolboxMain.cpp
		GridWidget.cpp	
		PluginServices.cpp
		UserInterfaceService.cpp
		FileUtil.cpp
		WindowFactory.cpp
		QTMainWindow.cpp
		ModelService.cpp
		RenderService.cpp
		FileReader2Motion.cpp
		FileChunkReader.cpp
		../anim/Animation.cpp
		../anim/AnimationService.cpp
		../OsgExtensions/Model.cpp
		../OsgExtensions/Mesh.cpp
		../OsgExtensions/Intersection.cpp
		../utils/ConfigFileManage/ConfigBuffer.cpp
		../utils/ConfigFileManage/ConfigurationGroup.cpp
		../utils/ConfigFileManage/FileReader.cpp
		../utils/ConfigFileManage/Parameter.cpp
		../utils/ConfigFileManage/ConfigurationFileService.cpp
		
		../OsgExtensions/SkeletonNode.cpp
		../OsgExtensions/AnimationNode.cpp
		../OsgExtensions/AnimationGroup.cpp	
		
		../OsgExtensions/tinyXML/tinystr.cpp
		../OsgExtensions/tinyXML/tinyxml.cpp
		../OsgExtensions/tinyXML/tinyxmlerror.cpp
		../OsgExtensions/tinyXML/tinyxmlparser.cpp
		
		../OsgExtensions/FModel.cpp
		
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
		ConsoleWidget.cpp
		Logger.cpp
		main.cpp
		GridWidget.cpp	
		QOSGWidget.cpp
		ServiceManager.cpp
		TimeLine.cpp
		ToolboxMain.cpp
		GridWidget.cpp	
		PluginServices.cpp
		UserInterfaceService.cpp
		FileUtil.cpp
		WindowFactory.cpp
		QTMainWindow.cpp
		ModelService.cpp
		RenderService.cpp
		FileReader2Motion.cpp
		FileChunkReader.cpp
		../anim/Animation.cpp
		../anim/AnimationService.cpp
		../OsgExtensions/Model.cpp
		../OsgExtensions/Mesh.cpp
		../OsgExtensions/Intersection.cpp
		../utils/ConfigFileManage/ConfigBuffer.cpp
		../utils/ConfigFileManage/ConfigurationGroup.cpp
		../utils/ConfigFileManage/FileReader.cpp
		../utils/ConfigFileManage/Parameter.cpp
		../utils/ConfigFileManage/ConfigurationFileService.cpp
		
		../OsgExtensions/SkeletonNode.cpp
		../OsgExtensions/AnimationNode.cpp
		../OsgExtensions/AnimationGroup.cpp	
		
		../OsgExtensions/tinyXML/tinystr.cpp
		../OsgExtensions/tinyXML/tinyxml.cpp
		../OsgExtensions/tinyXML/tinyxmlerror.cpp
		../OsgExtensions/tinyXML/tinyxmlparser.cpp
		
		../OsgExtensions/FModel.cpp
		
		
		AdapterWidget.h
		ConsoleWidget.h
		Logger.h
		GridWidget.h
		QOSGWidget.h
		ServiceManager.h
		../include/IServiceManager.h
		TimeLine.h
		ToolboxMain.h
		../include/SimpleFunctors.h	
		../include/BaseDataTypes.h
		../include/IBaseService.h
		../include/GlobalServicesIDs.h
		PluginServices.h
		UserInterfaceService.h
		FileUtil.h
		WindowFactory.h
		QTMainWindow.h
		ModelService.h
		RenderService.h
		FileReader2Motion.h
		FileChunkReader.h
		../anim/Animation.h
		../anim/AnimationService.h
		../OsgExtensions/Model.h
		../OsgExtensions/Mesh.h
		../OsgExtensions/DataTransfer.h
		../OsgExtensions/Intersection.h
		../include/Vec3.h
		../OsgExtensions/AnimationNode.h
		../OsgExtensions/AnimationGroup.h
		../OsgExtensions/SkeletonNode.h
		
		../OsgExtensions/tinyXML/tinystr.h
		../OsgExtensions/tinyXML/tinyxml.h
		../OsgExtensions/FModel.h
		
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
		GridWidget.h
		QOSGWidget.h
		TimeLine.h
		ToolboxMain.h
		Animation.h
		../include/SimpleFunctors.h	
		../include/BaseDataTypes.h
		../include/IBaseService.h
		../include/GlobalServicesIDs.h
		FileUtil.h
		WindowFactory.h
		QTMainWindow.h
		../anim/Animation.h
		)
		
	SOURCE_GROUP("Source Files\\Toolbox" FILES  
		AdapterWidget.cpp
		ConsoleWidget.cpp
		Logger.cpp
		main.cpp
		GridWidget.cpp	
		QOSGWidget.cpp
		TimeLine.cpp
		ToolboxMain.cpp
		Animation.cpp
		GridWidget.cpp	
		FileUtil.cpp
		WindowFactory.cpp
		QTMainWindow.cpp
		../anim/Animation.cpp
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
		../TEST/AnimationService.h
		ServiceManager.h
		PluginServices.h
		UserInterfaceService.h
		ModelService.h
		RenderService.h
		../include/IServiceManager.h
	)
	
	SOURCE_GROUP("Source Files\\Services" FILES  
		../utils/ConfigFileManage/ConfigurationFileService.cpp
		../TEST/AnimationService.cpp
		ServiceManager.cpp
		PluginServices.cpp
		UserInterfaceService.cpp
		ModelService.cpp
		RenderService.cpp
	)
	
	SOURCE_GROUP("Header Files\\Model" FILES  
		../OsgExtensions/Model.h
		../OsgExtensions/Mesh.h
		../OsgExtensions/DataTransfer.h
		../OsgExtensions/Intersection.h
		../include/Vec3.h
		../OsgExtensions/AnimationNode.h
		../OsgExtensions/AnimationGroup.h
		../OsgExtensions/SkeletonNode.h
	)
	
	SOURCE_GROUP("Source Files\\Model" FILES  
		../OsgExtensions/Model.cpp
		../OsgExtensions/Mesh.cpp
		../OsgExtensions/Intersection.cpp
		../OsgExtensions/AnimationNode.cpp
		../OsgExtensions/AnimationGroup.cpp	
		../OsgExtensions/SkeletonNode.cpp
	)
	
	SOURCE_GROUP("Source Files\\File Reader" FILES  
		../OsgExtensions/tinyXML/tinystr.cpp
		../OsgExtensions/tinyXML/tinyxml.cpp
		../OsgExtensions/tinyXML/tinyxmlerror.cpp
		../OsgExtensions/tinyXML/tinyxmlparser.cpp
		../OsgExtensions/FModel.cpp
		FileChunkReader.cpp
		FileReader2Motion.cpp
	)
	
	SOURCE_GROUP("Header Files\\File Reader" FILES  
		../OsgExtensions/tinyXML/tinystr.h
		../OsgExtensions/tinyXML/tinyxml.h
		../OsgExtensions/FModel.h
		FileChunkReader.h
		FileReader2Motion.h
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
		COMMAND ${CMAKE_COMMAND} -E copy ARGS ../../binaries/win32/QT/debug/QtCored4.dll ../../bin/Win32/Debug/
		COMMAND ${CMAKE_COMMAND} -E copy ARGS ../../binaries/win32/QT/debug/QtGuid4.dll ../../bin/Win32/Debug/
		COMMAND ${CMAKE_COMMAND} -E copy ARGS ../../binaries/win32/QT/debug/QtOpenGLd4.dll ../../bin/Win32/Debug/
		COMMAND ${CMAKE_COMMAND} -E copy ARGS ../../binaries/win32/dae2Motion/Debug/dae2Motion_d.dll ../../bin/Win32/Debug/
		#COMMAND ${CMAKE_COMMAND} -E copy ARGS ../../binaries/win32/dae2Motion/Release/dae2Motion.dll ../../bin/Win32/Release/
	)  	
		
	copy_files(../../binaries/win32/osg/debug ../../bin/Win32/Debug)
	
	#prefix hack
	SET_TARGET_PROPERTIES(toolbox_app PROPERTIES
		PREFIX ../../../bin/Win32/$(ConfigurationName)/
		DEBUG_POSTFIX _d
	)
	
ELSEIF(UNIX)
	# post build event
	ADD_CUSTOM_COMMAND(
		TARGET copy
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ARGS toolbox_app ../../bin/Linux/toolbox_app
	)
ENDIF()
