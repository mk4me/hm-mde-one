###############################################################################

# Inicjuje proces wyszukiwania biblioteki.
macro(ON_PROJECT_ADDED name)

	# flaga aby mozna bylo uzyc projektu w makrach
	set(${name}_FOUND 1 PARENT_SCOPE)
	# kopiujemy includy
	set(${name}_INCLUDE_DIR ${DEFAULT_PROJECT_INCLUDES} PARENT_SCOPE)
	
	# co budujemy?
	get_target_property(BUILD_TYPE ${name} TYPE)
	
	# sprawdzamy to po to, �eby projekty zale�ne (klienci bibliotek, testy) 
	# r�wnie� widzia�y te same zale�ne biblioteki, co projekt
	if (BUILD_TYPE STREQUAL "STATIC_LIBRARY" OR BUILD_TYPE STREQUAL "SHARED_LIBRARY")
		# wystarczy doda� target, reszt� zajmie si� CMake
		# musimy zrobi� zmienn� po�rednicz�c�, �eby m�c iterowa� po xxx_LIBRARIES
		set(${name}_LIBRARIES_PROXY ${name} PARENT_SCOPE)
		set(${name}_LIBRARIES ${name}_LIBRARIES_PROXY PARENT_SCOPE)
	else()
		# to nie biblioteka wi�c r�cznie dodajemy bilioteki zale�ne; tutaj wa�ne:
		# do proxy kopiujemy sta�e reprezentuj�ce nazwy bibliotek, kt�rych warto�ci
		# odwo�uj� si� ju� do fizycznych plik�w
		set(${name}_LIBRARIES_PROXY)
		foreach(value ${DEFAULT_PROJECT_DEPENDENCIES})
			list(APPEND ${name}_LIBRARIES_PROXY ${${value}_LIBRARIES})
		endforeach()
		set(${name}_LIBRARIES ${${name}_LIBRARIES_PROXY} PARENT_SCOPE)
	endif()
	
	
	
endmacro(ON_PROJECT_ADDED)

###############################################################################

# Makro ustawiaj�ce pewn� opcj� konfiguracji.
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

# Makro dodaj�ce projekt
# Parametry
#	name Nazwa projektu (i podkatalogu katalogu)
#	dependencies Lista zale�no�ci
macro(ADD_PROJECT name dependencies)
	set(ADD_PROJECT_FAILED 0)
	set(ADD_PROJECT_MESSAGE "")
	# szukamy wszystkich brakuj�cych zale�no�ci
	foreach (value ${dependencies})
		if (NOT ${value}_FOUND)
			set(ADD_PROJECT_FAILED 1)
			set(ADD_PROJECT_MESSAGE ${value} ", " ${ADD_PROJECT_MESSAGE})
		endif()
	endforeach()
	# sprawdzamy
	if (ADD_PROJECT_FAILED)
		message(${name} " not included because dependencies are missing: " ${ADD_PROJECT_MESSAGE})
	else()
		#message("${name}: ${dependencies}")
		set (DEFAULT_PROJECT_DEPENDENCIES ${dependencies})
		set (DEFAULT_PROJECT_LIBS)
		set (DEFAULT_PROJECT_INCLUDES "${PROJECT_SOURCE_DIR};${PROJECT_BINARY_DIR}/${name};${PROJECT_BINARY_DIR};${MOTION_INCLUDE_ROOT}" )
		# kopiujemy dane
		foreach (value ${dependencies})
			#if (NOT ${createDependency})
			#	message("(${name}) RAW ${value} libraries: ${${value}_LIBRARIES}")
			#endif()
			#message("(${name}) RAW ${value} libraries: ${${value}_LIBRARIES}")
			foreach (value2 ${${value}_LIBRARIES})
				if (NOT ${createDependency})
					#message("(${name})Current library: ${value2} : ${${value2}} : ")
				endif()
				list(APPEND DEFAULT_PROJECT_LIBS ${${value2}})
			endforeach()
			list(APPEND DEFAULT_PROJECT_INCLUDES ${${value}_INCLUDE_DIR})
			# czy zdefiniowano katalog konfiguracyjny?
			if ( ${value}_INCLUDE_CONFIG_DIR )
				list(APPEND DEFAULT_PROJECT_INCLUDES ${${value}_INCLUDE_CONFIG_DIR})
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