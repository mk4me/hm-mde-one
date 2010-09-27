cmake_minimum_required(VERSION 2.6) 
project (EDR)

# dodatkowe modu³y
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")
include(MotionFindUtils)
include(MotionTargetUtils)

#---------------------------------------------------
# blok definicji dla CMake'a

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(EDR_LIBRARIES_ROOT "${PROJECT_SOURCE_DIR}/../lib" CACHE PATH "Location of libraries.")
set(EDR_INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/../include" CACHE PATH "Location of includes.")
set(EDR_LIBRARIES_INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/../include" CACHE PATH "Location of includes.")
set(EDR_ROOT "${PROJECT_SOURCE_DIR}")
set(EDR_BUILD_ROOT "${PROJECT_BINARY_DIR}")

# Blok definicje zale¿ne od platformy
if(WIN32)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
	set(EDR_LIBRARIES_PLATFORM "win32" CACHE STRING "Platform")
elseif(UNIX)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
	set(EDR_LIBRARIES_PLATFORM "linux" CACHE STRING "Platform")
else()
	message(FATAL_ERROR "Platform not supported.")
endif()

set(EDR_LIBRARIES_ROOT_DEBUG "${EDR_LIBRARIES_ROOT}/${EDR_LIBRARIES_PLATFORM}/debug" CACHE PATH "Location of debug libraries")
set(EDR_LIBRARIES_ROOT_RELEASE "${EDR_LIBRARIES_ROOT}/${EDR_LIBRARIES_PLATFORM}/release" CACHE PATH "Location of release libraries")

# tylko debug/release - nie dzia³a!
# set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Typy konfiguracji" FORCE)

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
option(EDR_COPY_MODULES "Copy runtime modules into bin folder?" OFF)
# Opcje konfiguracji
CONFIG_OPTION(VM_ENABLE_EXCEPTIONS "Enable exeptions?" ON)

set(SOURCEGROUP_PRIVATE_HEADERS "Header files" CACHE STRING "Filter for private headers.")
set(SOURCEGROUP_SOURCES "Source files" CACHE STRING "Filter for sources.")
set(SOURCEGROUP_PUBLIC_HEADERS "Header files" CACHE STRING "Filter for public headers.")
set(SOURCEGROUP_UI "UI" CACHE STRING "Filter for .ui files and generated headers.")

option(EDR_VEBOSE_CONFIG "Print verbose info?" OFF)
set(FIND_VERBOSE ${EDR_VEBOSE_CONFIG})

#---------------------------------------------------
# wyszukanie potrzebnych bibliotek

# Szukamy bibliotek
find_package(Boost)
find_package(FFmpeg)
find_package(OSG)
find_package(Stdint)
find_package(Qt)
find_package(TinyXML)
#find_package(Prerequisites)
if (EDR_ENABLE_TESTS)
	find_package(CppUnit)
endif()

#---------------------------------------------------
# obs³uga kwestii modu³ów (so dynamicznie wczytywanych oraz dll)

if (EDR_COPY_MODULES)
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

	message("Copying finished. You should turn of EDR_COPY_MODULES option now.")
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
# nag³ówki
#set(EDR_INCLUDE_ROOT "../include/" CACHE PATH "Location of data.")
#install(DIRECTORY ${EDR_DATA_ROOT} DESTINATION bin/ FILES_MATCHING PATTERN "*.png") 

#---------------------------------------------------
# Projekty sk³adaj¹ce siê z podprojektów dodajamy makrem ADD_PROJECTS,
# natiomast pojedyncze - ADD_PROJECT
# ADD_PROJECT(template [{zalezne_projekty}])
# ADD_PROJECTS(template [{zalezne_projekty}])

#ADD_PROJECT(utils "BOOST")
#ADD_PROJECTS(vm)
#ADD_PROJECT(tm "OSG;utils")
#ADD_PROJECT(main "OSG;vm;tm")

#ADD_PROJECT(anim "")
#ADD_PROJECT(main "")

ADD_PROJECT(utils "BOOST")
ADD_PROJECTS(plugins)
ADD_PROJECT(core "utils;OSG;QT;TINYXML;plugins")




#---------------------------------------------------
# zaspby
set(EDR_DATA_ROOT "${PROJECT_SOURCE_DIR}/../data/" CACHE PATH "Location of data.")
install(DIRECTORY ${EDR_DATA_ROOT} DESTINATION bin/ FILES_MATCHING PATTERN "*.png") 

#---------------------------------------------------
# testy
option(EDR_ENABLE_TESTS "Build test projects?" OFF)
if (EDR_ENABLE_TESTS)
	set(EDR_TESTS_ROOT "../tests/" CACHE PATH "Location of data.")
	add_subdirectory(${EDR_TESTS_ROOT} ${PROJECT_BINARY_DIR}/tests)
endif()

#---------------------------------------------------
# projekt zasobów
option(EDR_ENABLE_SVN_RESOURCES "Enable svn resources?" OFF)
if(EDR_ENABLE_SVN_RESOURCES)
	set(EDR_SVN_RESOURCES_ROOT "${PROJECT_SOURCE_DIR}/../resources/" CACHE PATH "Location of resources being part of repository.")
	add_subdirectory(${EDR_SVN_RESOURCES_ROOT} ${PROJECT_BINARY_DIR}/resources)
	install(DIRECTORY ${EDR_SVN_RESOURCES_ROOT} DESTINATION bin/data/ 
		PATTERN ".svn" EXCLUDE
		PATTERN "CMakeLists.txt" EXCLUDE)
endif()

#---------------------------------------------------
# CPack
option(ENABLE_CPACK "Set on to enable CPack" OFF)
#wlaczamy CTest
include(CTest)
if(ENABLE_CPACK) 
	#konfigurujemy cpacka w zaleznosci od systemu operacyjnego
	if(WIN32)
		set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ${CMAKE_MODULE_PATH})
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

