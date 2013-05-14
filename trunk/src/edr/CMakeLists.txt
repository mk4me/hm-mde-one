cmake_minimum_required(VERSION 2.8)
include("../../CMakeModules/Logic/ProjectUtils.cmake")

INITIALIZE_SOLUTION( EDR )

ADD_EXTERNAL_SOLUTION_FINDERS(edrutils)

# projekt zasobów
set(SOLUTION_SVN_RESOURCES_ROOT "${PROJECT_SOURCE_DIR}/resources" CACHE PATH "Location of resources being part of repository.")

#---------------------------------------------------
# Dodatkowa konfiguracja CPack dla instalatora
set(CPACK_INSTALLER_RESOURCES "${PROJECT_SOURCE_DIR}/../installer_resources" CACHE PATH "Location of installer resources.")
set(CPACK_SOLUTION_SVN_RESOURCES_ROOT ${SOLUTION_SVN_RESOURCES_ROOT})
#---------------------------------------------------

FINALIZE_SOLUTION()
