cmake_minimum_required(VERSION 2.8)

project(EDR_UTILS)
include("../../CMakeModules/Logic/ProjectUtils.cmake")
INITIALIZE_SOLUTION("BOOST;TINYXML;OSG;FFMPEG;STDINT;QT;EIGEN3;BTK;CURL;CPPUNIT;OPENTHREADS")
message("T4")
# Czy skopiowaæ modu³y (niezlinkowane .so oraz .dll)?
#option(PROJECT_ENABLE_EXAMPLES "Build examples?" OFF)
#option(PROJECT_ENABLE_TESTS "Build test projects?" OFF)
# Opcje konfiguracji
CONFIG_OPTION(VIDLIB_ENABLE_LOG4CXX "Enable logging with log4cxx?" OFF)
CONFIG_OPTION(VIDLIB_ENABLE_OSG "Enable logging with log4cxx?" ON)
message("T5")
FINALIZE_SOLUTION()
message("T6")