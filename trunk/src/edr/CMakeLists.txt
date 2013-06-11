cmake_minimum_required(VERSION 2.8.10)
include("../../CMakeModules/Logic/SolutionUtils.cmake")

INITIALIZE_SOLUTION( MDE )

ADD_EXTERNAL_SOLUTION_FINDERS(edrutils)

FINALIZE_SOLUTION()
