cmake_minimum_required(VERSION 2.8)
include("../../CMakeModules/Logic/ProjectUtils.cmake")

INITIALIZE_SOLUTION( EDR )

# projekt zasobów
set(SOLUTION_SVN_RESOURCES_ROOT "${PROJECT_SOURCE_DIR}/resources" CACHE PATH "Location of resources being part of repository.")
option(SOLUTION_ENABLE_SVN_RESOURCES "Enable svn resources?" ON)
# if(SOLUTION_ENABLE_SVN_RESOURCES)	
	# add_subdirectory(${SOLUTION_SVN_RESOURCES_ROOT} ${PROJECT_BINARY_DIR}/resources)
	# install(DIRECTORY ${SOLUTION_SVN_RESOURCES_ROOT}/deploy/ DESTINATION bin/resources/ COMPONENT resources
		# PATTERN ".svn" EXCLUDE
		# PATTERN "CMakeLists.txt" EXCLUDE
		# PATTERN "log.ini" EXCLUDE
		# PATTERN "*.ts" EXCLUDE)
# endif()

#---------------------------------------------------
# Dodatkowa konfiguracja CPack dla instalatora
set(CPACK_INSTALLER_RESOURCES "${PROJECT_SOURCE_DIR}/../installer_resources" CACHE PATH "Location of installer resources.")
set(CPACK_SOLUTION_SVN_RESOURCES_ROOT ${SOLUTION_SVN_RESOURCES_ROOT})
#---------------------------------------------------

FINALIZE_SOLUTION()
