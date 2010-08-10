PROJECT(OSGDBPLUGIN)

# just debug / release
SET(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ${OSGDBPLUGIN_SOURCE_DIR} ${OSGDBPLUGIN_BINARY_DIR})

# INCLUDES
SET(OSGDBPLUGIN_INCLUDE_X
	../include
	../../include/osg
	../OsgExtensions/
)

IF(UNIX)

	# .cxx sources
	SET(OSGDBPLUGIN_SRCS_CXX
		FModel.cpp
		ReaderWriterPlugin.cpp
		FileChunkReader.cpp
		tinyXML/tinystr.cpp
		tinyXML/tinyxml.cpp
		tinyXML/tinyxmlerror.cpp
		tinyXML/tinyxmlparser.cpp
		
		../OsgExtensions/CustomGroup.cpp
		../OsgExtensions/SkeletonNode.cpp
		../OsgExtensions/AnimationNode.cpp
		../OsgExtensions/AnimationGroup.cpp
	)

	# LIBS
	SET(OSGDBPLUGIN_LIBS_X
		dl
		m
		pthread
		osg
		osgViewer
		osgSim
		osgUtil
	)

ELSEIF(WIN32)

	# .cxx sources
	SET(OSGDBPLUGIN_SRCS_CXX
		FModel.cpp
		ReaderWriterPlugin.cpp
		FileChunkReader.cpp
		tinyXML/tinystr.cpp
		tinyXML/tinyxml.cpp
		tinyXML/tinyxmlerror.cpp
		tinyXML/tinyxmlparser.cpp
		
		../OsgExtensions/CustomGroup.cpp
		../OsgExtensions/SkeletonNode.cpp
		../OsgExtensions/AnimationNode.cpp
		../OsgExtensions/AnimationGroup.cpp	

		FModel.h
		ModelData.h
		FAnimation.h
		ReaderWriterPlugin.h
		FileChunkReader.h
		tinyXML/tinystr.h
		tinyXML/tinyxml.h
	)
	
	# LIBS
	SET(OSGDBPLUGIN_LIBS_X
		debug ../../lib/win32/debug/osg/osgd
		debug ../../lib/win32/debug/osg/osgViewerd 
		debug ../../lib/win32/debug/osg/osgGAd
		debug ../../lib/win32/debug/osg/osgDBd
		debug ../../lib/win32/debug/osg/osgTextd
		debug ../../lib/win32/debug/osg/OpenThreadsd
		
		optimized ../../lib/win32/release/osg/osg
		optimized ../../lib/win32/release/osg/osgViewer
		optimized ../../lib/win32/release/osg/osgGA
		optimized ../../lib/win32/release/osg/osgDB
		optimized ../../lib/win32/release/osg/osgText
		optimized ../../lib/win32/release/osg/OpenThreads
	)
	
	
	SOURCE_GROUP("Header Files\\FileChunkReader" FILES  
		FileChunkReader.h
		)
		
	SOURCE_GROUP("Source Files\\FileChunkReader" FILES  
		FileChunkReader.cpp
		)

ENDIF()

INCLUDE_DIRECTORIES(${OSGDBPLUGIN_INCLUDE_X})

# srcs
IF(UNIX)
	ADD_LIBRARY(osgDB_tbs SHARED ${OSGDBPLUGIN_SRCS_CXX})
ELSEIF(WIN32)
	ADD_LIBRARY(osgDB_tbs SHARED ${OSGDBPLUGIN_SRCS_CXX})
ENDIF()

# libs
TARGET_LINK_LIBRARIES(osgDB_tbs ${OSGDBPLUGIN_LIBS_X})

IF(WIN32)
	# post build event
	ADD_CUSTOM_COMMAND(
		TARGET osgDB_tbs
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory ../../bin
		COMMAND ${CMAKE_COMMAND} -E make_directory ../../bin/Win32
		COMMAND ${CMAKE_COMMAND} -E make_directory ../../bin/Win32/$(ConfigurationName)
		COMMAND ${CMAKE_COMMAND} -E make_directory ../../bin/Win32/$(ConfigurationName)/osgPlugins-2.8.2
	)
	
	#prefix hack
	SET_TARGET_PROPERTIES(osgDB_tbs PROPERTIES
		PREFIX ../../../bin/Win32/$(ConfigurationName)/osgPlugins-2.8.2/
		DEBUG_POSTFIX d
	)
	
ELSEIF(UNIX)
	# post build event
	ADD_CUSTOM_COMMAND(
		TARGET osgDB_tbs
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ARGS libosgDB_tbs.so ../../bin/Linux/osgPlugins-2.8.2/osgdb_tbs.so
	)
ENDIF()

