cmake_minimum_required(VERSION 2.6) 
project (EDR)

# dodatkowe modu³y
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")
include(MotionFindUtils)
include(MotionTargetUtils)
set(CMAKE_USE_RELATIVE_PATHS TRUE)
#---------------------------------------------------
# blok definicji dla CMake'a

set(PROJECT_LIBRARIES_ROOT "${PROJECT_SOURCE_DIR}/../../../lib" CACHE PATH "Location of libraries.")
set(PROJECT_LIBRARIES_INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/../../../include" CACHE PATH "Location of includes.")
set(PROJECT_INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/../include" CACHE PATH "Location of includes.")
set(PROJECT_ROOT "${PROJECT_SOURCE_DIR}")
set(PROJECT_BUILD_ROOT "${PROJECT_BINARY_DIR}")
set(EDRUTILS_INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/../../edrutils/include" CACHE PATH "Location of edrutils public includes.")
set(EDRUTILS_BUILD_ROOT "${PROJECT_SOURCE_DIR}/../../../_out/edrutils" CACHE PATH "Location of edrutils project build.")

# Blok definicje zale¿ne od platformy
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
if(WIN32)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
	set(PROJECT_LIBRARIES_PLATFORM "win32" CACHE STRING "Platform")
	add_definitions(-D__WIN32__)
elseif(UNIX)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
	set(PROJECT_LIBRARIES_PLATFORM "linux" CACHE STRING "Platform")
	add_definitions(-D__UNIX__)
else()
	message(FATAL_ERROR "Platform not supported.")
endif()

set(PROJECT_LIBRARIES_ROOT_DEBUG "${PROJECT_LIBRARIES_ROOT}/${PROJECT_LIBRARIES_PLATFORM}/debug" CACHE PATH "Location of debug libraries")
set(PROJECT_LIBRARIES_ROOT_RELEASE "${PROJECT_LIBRARIES_ROOT}/${PROJECT_LIBRARIES_PLATFORM}/release" CACHE PATH "Location of release libraries")

# tylko debug/release - nie dzia³a!
# set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Typy konfiguracji" FORCE)

#---------------------------------------------------
# opcje

option(PROJECT_VEBOSE_CONFIG "Print verbose info?" OFF)
set(FIND_VERBOSE ${PROJECT_VEBOSE_CONFIG})
set(TARGET_VERBOSE ${PROJECT_VEBOSE_CONFIG})

# Opcje konfiguracji
CONFIG_OPTION(CORE_DISABLE_LOGGING "Disable logging?" OFF)
CONFIG_OPTION(CORE_ENABLE_LOG4CXX "Enable logging?" ON)
CONFIG_OPTION(CORE_ENABLE_LEAK_DETECTION "Enable leak detection?" OFF)

# Grupy Ÿróde³
set(SOURCEGROUP_PRIVATE_HEADERS "Header files" CACHE STRING "Filter for private headers.")
set(SOURCEGROUP_SOURCES "Source files" CACHE STRING "Filter for sources.")
set(SOURCEGROUP_PUBLIC_HEADERS "Header files" CACHE STRING "Filter for public headers.")
set(SOURCEGROUP_UI "UI" CACHE STRING "Filter for .ui files and generated headers.")

#---------------------------------------------------
# wyszukanie potrzebnych bibliotek

# konfiguracja modu³u wyszukuj¹cego
set(FIND_LIBRARIES_INCLUDE_ROOT ${PROJECT_LIBRARIES_INCLUDE_ROOT})
set(FIND_LIBRARIES_ROOT ${PROJECT_LIBRARIES_ROOT})
set(FIND_LIBRARIES_ROOT_DEBUG ${PROJECT_LIBRARIES_ROOT_DEBUG})
set(FIND_LIBRARIES_ROOT_RELEASE ${PROJECT_LIBRARIES_ROOT_RELEASE})
set(FIND_PLATFORM ${PROJECT_LIBRARIES_PLATFORM})

# Szukamy bibliotek
find_package(ZLIB)
find_package(BTK)
find_package(Boost)
find_package(OSG)
#find_package(Stdint)
find_package(ODE)
find_package(Qt)
find_package(QWT)
find_package(QuaZip)
find_package(TinyXML)
set(FFMPEG_ONLY_MODULES ON)
find_package(FFmpeg)
find_package(VidLib)
find_package(cURL)
find_package(wsdlpull)
find_package(log4cxx)
find_package(Utils)
find_package(osgui)
find_package(KinematicLib)
find_package(c3dlib)
find_package(dfmlib)
find_package(vdfmlib)
find_package(webserviceslib)
find_package(qthtmlgenlib)
find_package(TimelineLib)
find_package(TESTNODEFACTORY)
find_package(SQLITE3)
find_package(SQLCIPHER)
find_package(prerequisites)
# Biblioteczne wyszukanie OGL;
find_package(OpenGL)
set(OPENGL_LIBRARIES_PROXY ${OPENGL_LIBRARIES})
set(OPENGL_LIBRARIES OPENGL_LIBRARIES_PROXY)

#find_package(Prerequisites)

#---------------------------------------------------

# obs³uga modu³ów (.dll/.so)
option(PROJECT_COPY_MODULES "Copy runtime modules into bin folder?" ON)
if(PROJECT_COPY_MODULES)
	message("Copying modules")
	FIND_HANDLE_MODULES(PROJECT_COPY_MODULES)
	message("Copying finished. You should turn off option PROJECT_COPY_MODULES.")
endif()

set(PROJECT_REBUILD_DEPENDENCIES_DST "${PROJECT_SOURCE_DIR}/../../../" CACHE PATH "Location of rebuilt dependencies structure")
option(PROJECT_REBUILD_DEPENDENCIES "Rebuild dependencies?" OFF)
if(PROJECT_REBUILD_DEPENDENCIES)
	message("Rebuiling dependencies structure in ${PROJECT_REBUILD_DEPENDENCIES_DST}")
	FIND_REBUILD_DEPENDENCIES("${PROJECT_REBUILD_DEPENDENCIES_DST}")
	message("Rebuiling dependencies finished. You should turn off option PROJECT_REBUILD_DEPENDENCIES.")
endif()

#---------------------------------------------------
# Projekty sk³adaj¹ce siê z podprojektów dodajamy makrem ADD_PROJECTS,
# natiomast pojedyncze - ADD_PROJECT
# ADD_PROJECT(template [{zalezne_projekty}])
# ADD_PROJECTS(template)

if (CONFIG_CORE_ENABLE_LOG4CXX)
	ADD_PROJECT(corelib "KINEMATICLIB;LOG4CXX;BOOST;UTILS;OSG;OSGUI;QT;TINYXML")
else()
	ADD_PROJECT(corelib "KINEMATICLIB;BOOST;UTILS;OSG;OSGUI;QT;TINYXML")
endif()
ADD_PROJECT(core "BOOST;UTILS;OSG;QT;TINYXML;corelib;OPENGL;DFMLIB;VDFMLIB")
ADD_PROJECT(old_view "BOOST;UTILS;OSG;QT;TINYXML;core;corelib;OPENGL;DFMLIB;VDFMLIB;C3DLIB")
ADD_PROJECT(hmm_view "BOOST;UTILS;OSG;QT;TINYXML;core;corelib;OPENGL;DFMLIB;VDFMLIB;C3DLIB;QWT")
ADD_PROJECTS(plugins)

#---------------------------------------------------
# projekt zasobów
option(PROJECT_ENABLE_SVN_RESOURCES "Enable svn resources?" ON)
if(PROJECT_ENABLE_SVN_RESOURCES)
	set(PROJECT_SVN_RESOURCES_ROOT "${PROJECT_SOURCE_DIR}/../resources/" CACHE PATH "Location of resources being part of repository.")
	add_subdirectory(${PROJECT_SVN_RESOURCES_ROOT} ${PROJECT_BINARY_DIR}/resources)
	install(DIRECTORY ${PROJECT_SVN_RESOURCES_ROOT}/deploy/ DESTINATION bin/resources/ COMPONENT core
		PATTERN ".svn" EXCLUDE
		PATTERN "CMakeLists.txt" EXCLUDE
		PATTERN "log.ini" EXCLUDE
		PATTERN "*.ts" EXCLUDE)
	install(DIRECTORY ${PROJECT_LIBRARIES_ROOT}/win32/release/qt/plugins/ DESTINATION bin/plugins/ COMPONENT core 
		PATTERN "*.lib" EXCLUDE)
endif()


#---------------------------------------------------
# Dodatkowa konfiguracja CPack dla instalatora
set(CPACK_INSTALLER_RESOURCES "${PROJECT_SOURCE_DIR}/../../../installer_resources" CACHE PATH "Location of installer resources.")
set(CPACK_PROJECT_SVN_RESOURCES_ROOT ${PROJECT_SVN_RESOURCES_ROOT})
#---------------------------------------------------
# CPack
set(CPACK_PACKAGE_NAME "EDR")
set(CPACK_PACKAGE_FILE_NAME "EDR")
set(CPACK_NSIS_DISPLAY_NAME "EDR")
set(CPACK_NSIS_PACKAGE_NAME "EDR")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "EDR")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "EDR")
set(CPACK_COMPONENT_NEWCOMMUNICATION_GROUP "Plugins")
set(CPACK_COMPONENT_VIDEO_GROUP "Plugins")
set(CPACK_COMPONENT_C3D_GROUP "Plugins")
set(CPACK_COMPONENT_NEWCHART_GROUP "Plugins")
set(CPACK_COMPONENT_NEWTIMELINE_GROUP "Plugins")
set(CPACK_COMPONENT_NEWCOMMUNICATION_DISPLAY_NAME "Communication plugin")
set(CPACK_COMPONENT_VIDEO_DISPLAY_NAME "Video plugin")
set(CPACK_COMPONENT_NEWCHART_DISPLAY_NAME "Chart plugin")
set(CPACK_COMPONENT_C3D_DISPLAY_NAME "c3d plugin")
set(CPACK_COMPONENT_NEWTIMELINE_DISPLAY_NAME "Timeline plugin")
set(CPACK_COMPONENT_CORE_DISPLAY_NAME "Main application")
set(CPACK_COMPONENT_CORE_REQUIRED ON)
set(CPACK_COMPONENTS_ALL newCommunication c3d core newChart video newtimeline kinematic)
option(ENABLE_CPACK "Set on to enable CPack" OFF)
option(ENABLE_CTEST "Set on to enable CTest" OFF)
if(ENABLE_CTEST)
	#wlaczamy CTest
	include(CTest)
endif()
if(ENABLE_CPACK) 
	#konfigurujemy cpacka w zaleznosci od systemu operacyjnego
	if(WIN32)
		set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ${CMAKE_MODULE_PATH})
		set(CMAKE_MODULE_PATH "${CPACK_INSTALLER_RESOURCES}" ${CMAKE_MODULE_PATH})
	else()
		set(CPACK_BINARY_DEB ON)
		set(CPACK_BINARY_RPM OFF)
		set(CPACK_BINARY_STGZ OFF)
		set(CPACK_BINARY_TBZ2 OFF)
		set(CPACK_BINARY_TGZ OFF)
		set(CPACK_BINARY_TZ OFF)
		set(CPACK_PACKAGE_CONTACT "PJWSTK")
	endif()
	include(CPack)
endif(ENABLE_CPACK)

#---------------------------------------------------
# dodanie uninstalla

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

add_custom_target(UNINSTALL
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

 