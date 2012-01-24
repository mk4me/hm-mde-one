# przygotowanie do szukania
set(FIND_DISABLE_INCLUDES ON)
FIND_INIT(BTK btk)

FIND_STATIC_EXT(BTK_IO "BTKIO" "BTKIOd")
FIND_STATIC_EXT(BTK_Common "BTKCommon" "BTKCommond")
FIND_STATIC_EXT(BTK_Filters "BTKBasicFilters" "BTKBasicFiltersd")

# koniec
FIND_FINISH(BTK)

if (BTK_IO_FOUND AND BTK_Common_FOUND AND BTK_Filters_FOUND)

	# szukanie
	set(BTK_IO_INCLUDE_DIR "${FIND_LIBRARIES_INCLUDE_ROOT}/BTK/Code/IO" CACHE PATH "Location of stdint headers.")
	set(BTK_Common_INCLUDE_DIR "${FIND_LIBRARIES_INCLUDE_ROOT}/BTK/Code/Common" CACHE PATH "Location of stdint headers.")
	set(BTK_Filters_INCLUDE_DIR "${FIND_LIBRARIES_INCLUDE_ROOT}/BTK/Code/BasicFilters" CACHE PATH "Location of stdint headers.")
	set(BTK_Utilities_INCLUDE_DIR "${FIND_LIBRARIES_INCLUDE_ROOT}/BTK" CACHE PATH "Location of stdint headers.")
	set(BTK_Eigen2_INCLUDE_DIR "${FIND_LIBRARIES_INCLUDE_ROOT}/BTK/Utilities/eigen2" CACHE PATH "Location of stdint headers.")
	set(BTK_Config_DIR "${FIND_LIBRARIES_INCLUDE_ROOT}/BTK/" CACHE PATH "Location of stdint headers.")

	set(BTK_INCLUDE_DIR 
		"${BTK_IO_INCLUDE_DIR};${BTK_Common_INCLUDE_DIR};${BTK_Filters_INCLUDE_DIR};${BTK_Utilities_INCLUDE_DIR};${BTK_Eigen2_INCLUDE_DIR};${BTK_Config_DIR}"
		CACHE PATH "Location of stdint headers.")

	set(BTK_FOUND 1)

else()

	set(BTK_FOUND 0)

endif()