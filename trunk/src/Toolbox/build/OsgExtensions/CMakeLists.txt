PROJECT(OSG_EXTENSIONS)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ${OSG_EXTENSIONS_SOURCE_DIR} ${OSG_EXTENSIONS_BINARY_DIR})

IF(UNIX)

	# .cxx sources
	SET(OSG_EXTENSIONS_SRCS_CXX
		../../OsgExtensions/CustomGroup.cpp
		../../OsgExtensions/SkeletonNode.cpp
		../../OsgExtensions/AnimationNode.cpp		
		../../OsgExtensions/AnimationGroup.cpp		
		# TODO
		# to tutaj jest tymczasowo - az wszystko wczytam, wtedy te dane przestana byc potrzebne!
		../../osgDBPlugin/FModel.cpp
		../../osgDBPlugin/tinyXML/tinystr.cpp
		../../osgDBPlugin/tinyXML/tinyxml.cpp
		../../osgDBPlugin/tinyXML/tinyxmlerror.cpp
		../../osgDBPlugin/tinyXML/tinyxmlparser.cpp
	)

	# INCLUDES
	SET(OSG_EXTENSIONS_INCLUDE_X
		/usr/X11R6/include
	)

	# LIBS
	SET(OSG_EXTENSIONS_LIBS_X
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
	)

ELSEIF(WIN32)

	# .cxx sources
	SET(OSG_EXTENSIONS_SRCS_CXX
		../../OsgExtensions/CustomGroup.cpp
		../../OsgExtensions/SkeletonNode.cpp
		../../OsgExtensions/AnimationNode.cpp
		../../OsgExtensions/AnimationGroup.cpp	
		# TODO
		# to tutaj jest tymczasowo - az wszystko wczytam, wtedy te dane przestana byc potrzebne!
		../../osgDBPlugin/FModel.cpp
		../../osgDBPlugin/tinyXML/tinystr.cpp
		../../osgDBPlugin/tinyXML/tinyxml.cpp
		../../osgDBPlugin/tinyXML/tinyxmlerror.cpp
		../../osgDBPlugin/tinyXML/tinyxmlparser.cpp

		../../OsgExtensions/CustomGroup.h
		../../OsgExtensions/SkeletonNode.h
		../../OsgExtensions/AnimationNode.h		
		../../OsgExtensions/AnimationGroup.h		
		# TODO
		# jw
		#../../osgDBPlugin/FModel.h
		#../../osgDBPlugin/FMesh.h
		#../../osgDBPlugin/FAnimation.h
		#../../osgDBPlugin/tinyXML/tinystr.h
		#../../osgDBPlugin/tinyXML/tinyxml.h		
	)

	# INCLUDES
	SET(OSG_EXTENSIONS_INCLUDE_X
		debug
		../../include
		../../../../Tools/lua/include
	)

	# LIBS
	SET(OSG_EXTENSIONS_LIBS_X
		osgd.lib 
		osgViewerd.lib 
		osgGAd.lib 
		osgDBd.lib 
		osgTextd.lib 
		OpenThreadsd.lib 
		../../../../Tools/lua/lib/static/lua5.1
	)

ENDIF()

INCLUDE_DIRECTORIES(${OSG_EXTENSIONS_INCLUDE_X})

# srcs
ADD_LIBRARY(osgext ${OSG_EXTENSIONS_SRCS_CXX})

# libs
TARGET_LINK_LIBRARIES(osgext ${OSG_EXTENSIONS_LIBS_X})

IF(WIN32)
	# post build event
	ADD_CUSTOM_COMMAND(
		TARGET osgext
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ARGS $(ProjectDir)$(ConfigurationName)/osgext.lib $(ProjectDir)
	)
#ELSEIF(UNIX)
#	# post build event
#	ADD_CUSTOM_COMMAND(
#		TARGET osgext
#		COMMAND ${CMAKE_COMMAND} -E copy ARGS lib ../../../../bin/Linux/toolbox_app
#	)
ENDIF()

