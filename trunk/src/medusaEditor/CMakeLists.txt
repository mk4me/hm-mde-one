cmake_minimum_required(VERSION 2.8.11)
include("../../CMakeModules/Logic/SolutionUtils.cmake")

INITIALIZE_SOLUTION( MEDUSA )
ADD_EXTERNAL_SOLUTION_FINDERS(medusaLibraries)

FINALIZE_SOLUTION()
