###############################################################################

# Inicjuje proces wyszukiwania biblioteki.
macro(ON_PROJECT_ADDED name)
	
	# ustawiamy nazwe dla artefaktow wersji debug tak aby do nazwy na koniec by³o doklejane d, dla release bez zmian
	set_target_properties(${TARGET_TARGETNAME} PROPERTIES DEBUG_POSTFIX "d")
	
	# ustawiamy flagi projektu wynikaj¹ce z flag jego zale¿noœci
	list(REMOVE_DUPLICATES ${name}_COMPILER_DEFINITIONS)
	set_target_properties(${TARGET_TARGETNAME} PROPERTIES COMPILE_DEFINITIONS "${${name}_COMPILER_DEFINITIONS}")

	# flaga aby mozna bylo uzyc projektu w makrach
	set(${name}_FOUND 1 PARENT_SCOPE)
	# kopiujemy includy
	list(APPEND ${DEFAULT_PROJECT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR})
	set(${name}_INCLUDE_DIR ${DEFAULT_PROJECT_INCLUDES} CACHE STRING INTERNAL FORCE)
	#set(${name}_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
	list(APPEND ${name}_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
	# co budujemy?
	get_target_property(BUILD_TYPE ${name} TYPE)
	
	# NA RAZIE WY£ACZONY MECHANIZM PRZEKAZYWANIA BIBLIOTEK!
	# sprawdzamy to po to, ¿eby projekty zale¿ne (klienci bibliotek, testy) 
	# równie¿ widzia³y te same zale¿ne biblioteki, co projekt
	if (BUILD_TYPE STREQUAL "STATIC_LIBRARY" OR BUILD_TYPE STREQUAL "SHARED_LIBRARY")
		# wystarczy dodaæ target, reszt¹ zajmie siê CMake
		# musimy zrobiæ zmienn¹ poœrednicz¹c¹, ¿eby móc iterowaæ po xxx_LIBRARIES
		set(${name}_LIBRARIES_PROXY ${name} PARENT_SCOPE)
		#message(${${name}_LIBRARIES_PROXY})
		set(${name}_LIBRARIES ${name}_LIBRARIES_PROXY PARENT_SCOPE)
	else()
		# to nie biblioteka wiêc rêcznie dodajemy bilioteki zale¿ne; tutaj wa¿ne:
		# do proxy kopiujemy sta³e reprezentuj¹ce nazwy bibliotek, których wartoœci
		# odwo³uj¹ siê ju¿ do fizycznych plików
		set(${name}_LIBRARIES_PROXY)
		foreach(value ${DEFAULT_PROJECT_DEPENDENCIES})
			list(APPEND ${name}_LIBRARIES_PROXY ${${value}_LIBRARIES})
		endforeach()
		# // rev - przywrocona linijka ponizej
		set(${name}_LIBRARIES ${${name}_LIBRARIES_PROXY} PARENT_SCOPE)
	endif()
	
	
	
endmacro(ON_PROJECT_ADDED)

###############################################################################

# Makro ustawiaj¹ce pewn¹ opcjê konfiguracji.
# Parametry:
#	name	Nazwa makra.
#	info	Tekstowa informacja o opcji.
#	default	ON / OFF
macro(CONFIG_OPTION name info default)
	option(CONFIG_${name} ${info} ${default})
	if (CONFIG_${name})
		set(${name} ${name})
	endif()
endmacro(CONFIG_OPTION)

###############################################################################
# Makro generuje wykonywalny skrypt z ustawionymi sciezkami do bibliotek
macro(GENERATE_UNIX_EXECUTABLE_SCRIPT)
	if (UNIX)
		set (script "\#!/bin/sh\nexport LD_LIBRARY_PATH=")
		  
		foreach(value ${DEFAULT_PROJECT_DEPENDENCIES})
			set(dir "${${value}_LIBRARY_DIR_RELEASE}")
			set (script "${script}${dir}:")
		endforeach()

		set( script "${script}:$LD_LIBRARY_PATH\nexec ./${TARGET_TARGETNAME} $*")
		set( filename "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/run_${TARGET_TARGETNAME}.sh")
		file (WRITE ${filename} "${script}")
		execute_process(COMMAND chmod a+x ${filename})
	endif()
endmacro(GENERATE_UNIX_EXECUTABLE_SCRIPT)

###############################################################################

# Makro dodaj¹ce projekt
# Parametry
#	name Nazwa projektu (i podkatalogu katalogu)
#	dependencies Lista zale¿noœci
macro(ADD_PROJECT name dependencies)
	set(ADD_PROJECT_FAILED 0)
	set(ADD_PROJECT_MESSAGE "")
	set(${name}_COMPILER_DEFINITIONS "")
	# szukamy wszystkich brakuj¹cych zale¿noœci
	foreach (value ${dependencies})
		if (NOT ${value}_FOUND)
			set(ADD_PROJECT_FAILED 1)
			set(ADD_PROJECT_MESSAGE ${value} ", " ${ADD_PROJECT_MESSAGE})
			TARGET_NOTIFY(${name} "${value} not found")
		endif()
	endforeach()
	# sprawdzamy
	if (ADD_PROJECT_FAILED)
		message(${name} " not included because dependencies are missing: " ${ADD_PROJECT_MESSAGE})
	else()
		#message("${name}: ${dependencies}")
		set (DEFAULT_PROJECT_DEPENDENCIES ${dependencies})
		set (DEFAULT_PROJECT_LIBS)
		set (DEFAULT_PROJECT_INCLUDES "${PROJECT_BINARY_DIR}/${name};${PROJECT_BINARY_DIR}/src/${name};${PROJECT_BUILD_ROOT};${PROJECT_INCLUDE_ROOT};${PROJECT_ROOT}" )
		# kopiujemy dane
		foreach (value ${dependencies})
			#if (NOT ${createDependency})
			#	message("(${name}) RAW ${value} libraries: ${${value}_LIBRARIES}")
			#endif()
			TARGET_NOTIFY(${name} "RAW DEPENDENCY ${value} libraries: ${${value}_LIBRARIES}")
			
			foreach (value2 ${${value}_LIBRARIES})
			#message("(${name})Current library: ${value2} : ${${value2}} : ")
				if (NOT ${createDependency})
					#message("(${name})Current library: ${value2} : ${${value2}} : ")
				endif()
				list(APPEND DEFAULT_PROJECT_LIBS ${${value2}})
			endforeach()
			
			#foreach (value3 ${${value}_INCLUDE_DIR})
			#list(APPEND DEFAULT_PROJECT_INCLUDES ${${value3}})
			list(APPEND DEFAULT_PROJECT_INCLUDES ${${value}_INCLUDE_DIR})
			#endforeach()
			
			# czy zdefiniowano katalog konfiguracyjny?
			if ( ${value}_INCLUDE_CONFIG_DIR )
				list(APPEND DEFAULT_PROJECT_INCLUDES ${${value}_INCLUDE_CONFIG_DIR})
			endif()
			
			if ( ${value}_CUSTOM_COMPILER_DEFINITIONS )
				#foreach (value4 ${value}_CUSTOM_COMPILER_DEFINITIONS})
					#add_definitions(${${value}_CUSTOM_COMPILER_DEFINITIONS})
					list(APPEND ${name}_COMPILER_DEFINITIONS ${${value}_CUSTOM_COMPILER_DEFINITIONS})
				#endforeach()
			endif()
			
		endforeach()

		add_subdirectory(${name})
	endif()
endmacro(ADD_PROJECT)

###############################################################################

macro(ADD_PROJECTS name)

	add_subdirectory(${name})
	
endmacro(ADD_PROJECTS)

###############################################################################

MACRO(SET_PRECOMPILED_HEADER header source sources)
	if(MSVC)
		get_filename_component(_basename ${header} NAME_WE)
		set(_binary "${CMAKE_CURRENT_BINARY_DIR}/${_basename}.pch")
		set_source_files_properties(${source}
			PROPERTIES COMPILE_FLAGS "/Yc\"${header}\" /Fp\"${_binary}\""
			OBJECT_OUTPUTS "${_binary}")
		set_source_files_properties(${sources}
			PROPERTIES COMPILE_FLAGS "/Yu\"${_binary}\" /FI\"${_binary}\" /Fp\"${_binary}\""
			OBJECT_DEPENDS "${_binary}")  
	else()
		list(APPEND ${PROJECT_NAME}_COMPILER_DEFINITIONS DISABLE_PRECOMPILED_HEADERS)
		#add_definitions(-DDISABLE_PRECOMPILED_HEADERS)
	endif()
ENDMACRO(SET_PRECOMPILED_HEADER)

###############################################################################

macro(TARGET_NOTIFY var msg)
	if (TARGET_VERBOSE)
		message(STATUS "TARGET>${var}>${msg}")
	endif()
endmacro()

