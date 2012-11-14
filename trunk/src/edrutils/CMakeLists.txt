cmake_minimum_required(VERSION 2.8)
project(EDR_UTILS)
include("../../CMakeModules/Logic/ProjectUtils.cmake")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CustomCMakeModules;${CMAKE_MODULE_PATH}")

INITIALIZE_SOLUTION("BOOST;TINYXML;OSG;FFMPEG;STDINT;QT;Eigen3;BTK;CURL")

# Czy skopiowaæ modu³y (niezlinkowane .so oraz .dll)?
#option(PROJECT_ENABLE_EXAMPLES "Build examples?" OFF)
#option(PROJECT_ENABLE_TESTS "Build test projects?" OFF)
# Opcje konfiguracji
CONFIG_OPTION(VIDLIB_ENABLE_LOG4CXX "Enable logging with log4cxx?" OFF)
CONFIG_OPTION(VIDLIB_ENABLE_OSG "Enable logging with log4cxx?" ON)

FINALIZE_SOLUTION()
