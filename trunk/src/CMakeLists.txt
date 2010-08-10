# project name
PROJECT(Framework_Solution)

# 2.4.3 is actual minimum, but you have to add additional hacks
# uncomment this if you need 2.4.3 compatibility
# if(COMMAND cmake_policy)
#	 cmake_policy(SET CMP0003 NEW)
# endif(COMMAND cmake_policy)


IF(WIN32)
	ADD_DEFINITIONS(-D__WIN32__)
ELSEIF(UNIX)
	ADD_DEFINITIONS(-D__UNIX__)
ENDIF()

CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

# just debug / release
SET(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

SET(TOOLBOX_BIN_DIR ../../bin/)

# find and setup Qt4 for this project
FIND_PACKAGE(Qt4 REQUIRED)

# tell cmake to process CMakeLists.txt in these subdirs
# SUBDIRS(OsgExtensions ControlPlugin osgDBPlugin Toolbox)
SUBDIRS(anim osgDBPlugin main)

#---------------------------------------------------

option(ENABLE_CPACK "Set on to enable CPack" OFF)

#wlaczamy CTest
include(CTest)

if(ENABLE_CPACK) 
#konfigurujemy cpacka w zaleznosci od systemu operacyjnego

if(WIN32 AND NOT UNIX)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ${CMAKE_MODULE_PATH})

else(WIN32 AND NOT UNIX)

set(CPACK_BINARY_DEB ON)
set(CPACK_BINARY_RPM OFF)
set(CPACK_BINARY_STGZ OFF)
set(CPACK_BINARY_TBZ2 OFF)
set(CPACK_BINARY_TGZ OFF)
set(CPACK_BINARY_TZ OFF)
set(CPACK_PACKAGE_CONTACT "PJWSTK")

endif(WIN32 AND NOT UNIX)
include(CPack)
endif(ENABLE_CPACK)

#---------------------------------------------------