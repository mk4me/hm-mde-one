cmake_minimum_required(VERSION 2.6) 
project (EDRUtils)

# dodatkowe modu³y
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")
include(MotionFindUtils)
include(MotionTargetUtils)

#---------------------------------------------------
# blok definicji dla CMake'a

set(PROJECT_LIBRARIES_ROOT "${PROJECT_SOURCE_DIR}/../../../lib" CACHE PATH "Location of libraries.")
set(PROJECT_LIBRARIES_INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/../../../include" CACHE PATH "Location of includes.")
set(PROJECT_INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/../include" CACHE PATH "Location of includes.")
set(PROJECT_ROOT "${PROJECT_SOURCE_DIR}")
set(PROJECT_BUILD_ROOT "${PROJECT_BINARY_DIR}")

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
	set(PROJECT_LIBRARIES_PLATFORM "linux32" CACHE STRING "Platform")
	add_definitions(-D__UNIX__ -std=c++0x -fpermissive)
else()
	message(FATAL_ERROR "Platform not supported.")
endif()

set(PROJECT_LIBRARIES_ROOT_DEBUG "${PROJECT_LIBRARIES_ROOT}/${PROJECT_LIBRARIES_PLATFORM}/debug" CACHE PATH "Location of debug libraries")
set(PROJECT_LIBRARIES_ROOT_RELEASE "${PROJECT_LIBRARIES_ROOT}/${PROJECT_LIBRARIES_PLATFORM}/release" CACHE PATH "Location of release libraries")

#---------------------------------------------------
# opcje

option(PROJECT_VEBOSE_CONFIG "Print verbose info?" OFF)
set(FIND_VERBOSE ${PROJECT_VEBOSE_CONFIG})
set(TARGET_VERBOSE ${PROJECT_VEBOSE_CONFIG})

# Czy skopiowaæ modu³y (niezlinkowane .so oraz .dll)?
option(PROJECT_ENABLE_EXAMPLES "Build examples?" OFF)
option(PROJECT_ENABLE_TESTS "Build test projects?" OFF)

# Opcje konfiguracji
CONFIG_OPTION(VIDLIB_ENABLE_LOG4CXX "Enable logging with log4cxx?" OFF)
CONFIG_OPTION(VIDLIB_ENABLE_OSG "Enable logging with log4cxx?" ON)

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
find_package(Boost)
find_package(FFmpeg)
find_package(OSG)
find_package(Stdint)
find_package(log4cxx)
find_package(TinyXML)
find_package(Prerequisites)
find_package(Qt)
find_package(Qwt)
#find_package(ODE)
find_package(BTK)
find_package(cURL)
#find_package(wsdlpull)

if (PROJECT_ENABLE_TESTS)
	find_package(CppUnit)
endif()

#---------------------------------------------------

# obs³uga modu³ów (.dll/.so)
option(PROJECT_COPY_MODULES "Copy runtime modules into bin folder?" OFF)
if(PROJECT_COPY_MODULES)
	message("Copying modules")
	FIND_HANDLE_MODULES(PROJECT_COPY_MODULES)
	message("Copying finished. You should turn off option PROJECT_COPY_MODULES.")
endif()

set(PROJECT_REBUILD_DEPENDENCIES_DST "${PROJECT_SOURCE_DIR}/../../" CACHE PATH "Location of rebuilt dependencies structure")
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
# ADD_PROJECTS(template [{zalezne_projekty}])

ADD_PROJECT(utils "BOOST")

set(VIDLIB_DEPENDENCIES)
if ( CONFIG_VIDLIB_ENABLE_OSG )
	list(APPEND VIDLIB_DEPENDENCIES "OSG")
endif()
if ( CONFIG_VIDLIB_ENABLE_LOG4CXX )
	list(APPEND VIDLIB_DEPENDENCIES "LOG4CXX")
endif()
ADD_PROJECT(vidlib "BOOST;FFMPEG;utils;${VIDLIB_DEPENDENCIES};STDINT")
ADD_PROJECT(kinematiclib "BOOST;TINYXML;OSG;utils")
ADD_PROJECT(osgui "utils;OSG;QT;BOOST")
ADD_PROJECT(dfmlib "BOOST;OSG;utils")
ADD_PROJECT(vdfmlib "osgui;dfmlib;BOOST;OSG;utils")
ADD_PROJECT(testnodefactory "dfmlib;BOOST;utils")
ADD_PROJECT(timelinelib "OSG;utils;BOOST;QT")
ADD_PROJECT(c3dlib "BTK;BOOST;OSG")
ADD_PROJECT(webserviceslib "BOOST;TINYXML;CURL;OSG")
ADD_PROJECT(qthtmlgenlib "BOOST;TINYXML;QT;utils")
#ADD_PROJECT(asfamctoquatcsv "kinematiclib;OSG;BOOST;TINYXML;utils")

#---------------------------------------------------
# przyk³ady

if (PROJECT_ENABLE_EXAMPLES)
	set(PROJECT_EXAMPLES_ROOT "${PROJECT_SOURCE_DIR}/../examples/" CACHE PATH "Location of data.")
	add_subdirectory(${PROJECT_EXAMPLES_ROOT} ${PROJECT_BINARY_DIR}/examples)
endif()

#---------------------------------------------------
# testy
if (PROJECT_ENABLE_TESTS)
 include(CTest)
 set(GLOBAL_TESTS_ROOT "../tests/" CACHE PATH "Location of data.")
 add_subdirectory(${GLOBAL_TESTS_ROOT} ${PROJECT_BINARY_DIR}/tests)
 ENABLE_TESTING()
 ADD_TEST(vidlib_test ${PROJECT_BINARY_DIR}/bin/Release/vidlib_test)
 SET_TESTS_PROPERTIES(vidlib_test PROPERTIES PASS_REGULAR_EXPRESSION "OK")
 ADD_TEST(utils_test ${PROJECT_BINARY_DIR}/bin/Release/utils_test)
 SET_TESTS_PROPERTIES(utils_test PROPERTIES PASS_REGULAR_EXPRESSION "OK")
 ADD_TEST(kinematiclib_test ${PROJECT_BINARY_DIR}/bin/Release/kinematiclib_test)
 SET_TESTS_PROPERTIES(kinematiclib_test PROPERTIES PASS_REGULAR_EXPRESSION "OK")
 ADD_TEST(dfmlib_test ${PROJECT_BINARY_DIR}/bin/Release/dfmlib_test)
 SET_TESTS_PROPERTIES(dfmlib_test PROPERTIES PASS_REGULAR_EXPRESSION "OK")
endif()

#if (PROJECT_ENABLE_TESTS)
#	set(PROJECT_TESTS_ROOT "${PROJECT_SOURCE_DIR}/../tests/" CACHE PATH "Location of data.")
#	add_subdirectory(${PROJECT_TESTS_ROOT} ${PROJECT_BINARY_DIR}/tests)
#endif()
#
#---------------------------------------------------
# dodanie uninstalla

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

add_custom_target(UNINSTALL
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

