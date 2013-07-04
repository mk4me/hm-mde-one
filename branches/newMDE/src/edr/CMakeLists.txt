cmake_minimum_required(VERSION 2.8)
include("../../CMakeModules/Logic/ProjectUtils.cmake")

INITIALIZE_SOLUTION( EDR )
ADD_EXTERNAL_SOLUTION_FINDERS(edrutils)

FINALIZE_SOLUTION()
