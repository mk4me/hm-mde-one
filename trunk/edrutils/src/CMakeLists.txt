cmake_minimum_required(VERSION 2.6) 
project (EDRUtils)

# dodatkowe modu³y
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")
include(MotionFindUtils)
include(MotionTargetUtils)

#---------------------------------------------------
# blok definicji dla CMake'a

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(GLOBAL_LIBRARIES_ROOT "${PROJECT_SOURCE_DIR}/../libs/lib" CACHE PATH "Location of libraries.")
set(GLOBAL_INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/../include" CACHE PATH "Location of includes.")
set(GLOBAL_LIBRARIES_INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/../libs/include" CACHE PATH "Location of includes.")
set(GLOBAL_ROOT "${PROJECT_SOURCE_DIR}")
set(GLOBAL_BUILD_ROOT "${PROJECT_BINARY_DIR}")

# Blok definicje zale¿ne od platformy
if(WIN32)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
	set(GLOBAL_LIBRARIES_PLATFORM "win32" CACHE STRING "Platform")
elseif(UNIX)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
	set(GLOBAL_LIBRARIES_PLATFORM "linux" CACHE STRING "Platform")
else()
	message(FATAL_ERROR "Platform not supported.")
endif()

set(GLOBAL_LIBRARIES_ROOT_DEBUG "${GLOBAL_LIBRARIES_ROOT}/${GLOBAL_LIBRARIES_PLATFORM}/debug" CACHE PATH "Location of debug libraries")
set(GLOBAL_LIBRARIES_ROOT_RELEASE "${GLOBAL_LIBRARIES_ROOT}/${GLOBAL_LIBRARIES_PLATFORM}/release" CACHE PATH "Location of release libraries")

set(FIND_LIBRARIES_INCLUDE_ROOT ${GLOBAL_LIBRARIES_INCLUDE_ROOT})
set(FIND_LIBRARIES_ROOT ${GLOBAL_LIBRARIES_ROOT})
set(FIND_LIBRARIES_ROOT_DEBUG ${GLOBAL_LIBRARIES_ROOT_DEBUG})
set(FIND_LIBRARIES_ROOT_RELEASE ${GLOBAL_LIBRARIES_ROOT_RELEASE})
set(FIND_PLATFORM ${GLOBAL_LIBRARIES_PLATFORM})

#---------------------------------------------------
# definicje dla Ÿróde³

if(WIN32)
	add_definitions(-D__WIN32__)
elseif(UNIX)
	add_definitions(-D__UNIX__)
endif()

#---------------------------------------------------
# opcje

# Czy skopiowaæ modu³y (niezlinkowane .so oraz .dll)?
option(GLOBAL_COPY_MODULES "Copy runtime modules into bin folder?" OFF)
option(GLOBAL_ENABLE_EXAMPLES "Build examples?" OFF)
option(GLOBAL_ENABLE_TESTS "Build test projects?" OFF)

# Opcje konfiguracji
CONFIG_OPTION(VIDLIB_ENABLE_LOG4CXX "Enable logging with log4cxx?" OFF)
CONFIG_OPTION(VIDLIB_ENABLE_OSG "Enable logging with log4cxx?" ON)


set(SOURCEGROUP_PRIVATE_HEADERS "Header files" CACHE STRING "Filter for private headers.")
set(SOURCEGROUP_SOURCES "Source files" CACHE STRING "Filter for sources.")
set(SOURCEGROUP_PUBLIC_HEADERS "Header files" CACHE STRING "Filter for public headers.")
set(SOURCEGROUP_UI "UI" CACHE STRING "Filter for .ui files and generated headers.")

option(GLOBAL_VEBOSE_CONFIG "Print verbose info?" OFF)
set(FIND_VERBOSE ${GLOBAL_VEBOSE_CONFIG})
set(TARGET_VERBOSE ${GLOBAL_VEBOSE_CONFIG})

#---------------------------------------------------
# wyszukanie potrzebnych bibliotek

# Szukamy bibliotek
find_package(Boost)
find_package(FFmpeg)
find_package(OSG)
find_package(Stdint)
find_package(log4cxx)
find_package(Prerequisites)
find_package(SDL)

if (GLOBAL_ENABLE_TESTS)
	find_package(CppUnit)
endif()

#---------------------------------------------------
# obs³uga kwestii modu³ów (so dynamicznie wczytywanych oraz dll)

if (GLOBAL_COPY_MODULES)
	message(STATUS "Copying modules...")
	# wybieramy listê konfiguracji
	if ( WIN32 )
		# lecimy po build typach
		foreach (buildType ${CMAKE_CONFIGURATION_TYPES})
			FIND_COPY_AND_INSTALL_MODULES(${buildType} ${buildType})
		endforeach()
	else()
		FIND_COPY_AND_INSTALL_MODULES("${CMAKE_BUILD_TYPE}" "")
	endif()
	message("Copying finished. You should turn of GLOBAL_COPY_MODULES option now.")
endif()
if ( UNIX )
	# http://www.cmake.org/Wiki/CMake_RPATH_handling
	# use, i.e. don't skip the full RPATH for the build tree
	set(CMAKE_SKIP_BUILD_RPATH  FALSE)
	# when building, don't use the install RPATH already
	# (but later on when installing)
	set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
	# the RPATH to be used when installing
	set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
	# add the automatically determined parts of the RPATH
	# which point to directories outside the build tree to the install RPATH
	set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
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
ADD_PROJECT(vidlib "FFMPEG;STDINT;utils;${VIDLIB_DEPENDENCIES}")

#---------------------------------------------------
# przyk³ady

if (GLOBAL_ENABLE_EXAMPLES)
	set(GLOBAL_EXAMPLES_ROOT "../examples/" CACHE PATH "Location of data.")
	add_subdirectory(${GLOBAL_EXAMPLES_ROOT} ${PROJECT_BINARY_DIR}/examples)
endif()

#---------------------------------------------------
# testy

if (GLOBAL_ENABLE_TESTS)
	set(GLOBAL_TESTS_ROOT "../tests/" CACHE PATH "Location of data.")
	add_subdirectory(${GLOBAL_TESTS_ROOT} ${PROJECT_BINARY_DIR}/tests)
endif()

#---------------------------------------------------
# dodanie uninstalla

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

add_custom_target(UNINSTALL
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

