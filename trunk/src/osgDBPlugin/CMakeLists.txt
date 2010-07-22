PROJECT(OSGDBPLUGIN)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ${OSGDBPLUGIN_SOURCE_DIR} ${OSGDBPLUGIN_BINARY_DIR})

# INCLUDES
SET(OSGDBPLUGIN_INCLUDE_X
	../include
	../OsgExtensions/
)

IF(UNIX)

	# .cxx sources
	SET(OSGDBPLUGIN_SRCS_CXX
		FModel.cpp
		ReaderWriterPlugin.cpp
		tinyXML/tinystr.cpp
		tinyXML/tinyxml.cpp
		tinyXML/tinyxmlerror.cpp
		tinyXML/tinyxmlparser.cpp
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
		osgext
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
		osgd.lib 
		osgViewerd.lib 
		osgGAd.lib 
		osgDBd.lib 
		osgTextd.lib 
		OpenThreadsd.lib
		$(ProjectDir)../lib/osgext.lib
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
	ADD_LIBRARY(osgDB_tbsd SHARED ${OSGDBPLUGIN_SRCS_CXX})
ELSEIF(WIN32)
	ADD_LIBRARY(osgDB_tbsd SHARED ${OSGDBPLUGIN_SRCS_CXX})
ENDIF()

# libs
TARGET_LINK_LIBRARIES(osgDB_tbsd ${OSGDBPLUGIN_LIBS_X})

IF(WIN32)
	# post build event
	ADD_CUSTOM_COMMAND(
		TARGET osgDB_tbsd
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory ../../bin
		COMMAND ${CMAKE_COMMAND} -E make_directory ../../bin/Win32
		COMMAND ${CMAKE_COMMAND} -E make_directory ../../bin/Win32/$(ConfigurationName)
		COMMAND ${CMAKE_COMMAND} -E make_directory ../../bin/Win32/$(ConfigurationName)/osgPlugins-2.8.2
		COMMAND ${CMAKE_COMMAND} -E make_directory ../main/$(ConfigurationName)/osgPlugins-2.8.2
		COMMAND ${CMAKE_COMMAND} -E copy ARGS $(ConfigurationName)/osgDB_tbsd.dll ../../bin/Win32/$(ConfigurationName)/osgPlugins-2.8.2/
		COMMAND ${CMAKE_COMMAND} -E copy ARGS $(ConfigurationName)/osgDB_tbsd.dll ../main/$(ConfigurationName)/osgPlugins-2.8.2/
	)
ELSEIF(UNIX)
	# post build event
	ADD_CUSTOM_COMMAND(
		TARGET osgDB_tbsd
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ARGS libosgDB_tbsd.so ../../bin/Linux/osgPlugins-2.8.2/osgdb_tbs.so
	)
ENDIF()

