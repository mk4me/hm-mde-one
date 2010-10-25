###############################################################################
# Inicjuje proces wyszukiwania biblioteki.
macro(FIND_INIT variable dirName)

	# g��wne �cie�ki
	if (NOT FIND_DISABLE_INCLUDES)
		set(${variable}_INCLUDE_DIR "${EDR_LIBRARIES_INCLUDE_ROOT}/${dirName}" CACHE PATH "Location of ${variable} headers.")
	endif()
	set(${variable}_LIBRARY_DIR_DEBUG "${EDR_LIBRARIES_ROOT_DEBUG}/${dirName}" CACHE PATH "Location of ${variable} debug libraries.")
	set(${variable}_LIBRARY_DIR_RELEASE "${EDR_LIBRARIES_ROOT_RELEASE}/${dirName}" CACHE PATH "Location of ${variable} libraries.")
	# lokalizacja bibliotek dla trybu debug
	set (FIND_DIR_DEBUG ${${variable}_LIBRARY_DIR_DEBUG})
	# lokalizacja bibliotek
	set (FIND_DIR_RELEASE ${${variable}_LIBRARY_DIR_RELEASE})
	# zerujemy list� wyszukanych bibliotek
	set (FIND_RESULTS)
	# mo�liwy przyrostek dla bibliotek w wersji debug
	set (FIND_DEBUG_SUFFIXES "d")
	
	# wyzerowanie zmiennych logicznych
	set (FIND_RESULTS_LOGICAL_OR 0)
	set (FIND_RESULTS_LOGICAL_AND 1)
	
	FIND_NOTIFY(${variable} "FIND_INIT: include: ${${variable}_INCLUDE_DIR}; debug: ${${variable}_LIBRARY_DIR_DEBUG}; release: ${${variable}_LIBRARY_DIR_RELEASE}")
	
endmacro(FIND_INIT)

###############################################################################

macro(FIND_INCLUDE_PLATFORM_HEADERS variable dirName)
	# okre�lamy �cie�k� do katalogu z nag��wkami konfiguracyjnymi
	set(${variable}_INCLUDE_CONFIG_DIR "${${variable}_INCLUDE_DIR}/../${EDR_LIBRARIES_PLATFORM}/${dirName}" 
		CACHE PATH "Location of config headers")
	FIND_NOTIFY(${variable} "FIND_INIT: platform headers: ${${variable}_INCLUDE_CONFIG_DIR}")
endmacro(FIND_INCLUDE_PLATFORM_HEADERS)

###############################################################################

# Ko�czy proces wyszukiwania biblioteki.
macro(FIND_FINISH variable)

	# skopiowanie
	set (${variable}_LIBRARIES ${FIND_RESULTS})	
	set (FIND_DISABLE_INCLUDES OFF)
	FIND_NOTIFY(${variable} "FIND_FINISH: found libraries ${FIND_RESULTS}")

endmacro(FIND_FINISH)



###############################################################################

# Makro wyszukuje bibliotek statycznych lub plik�w lib dla wsp�dzielonych bibliotek (windows).
# Zak�ada, �e istniej� dwie zmienne:
# FIND_DIR_DEBUG Miejsca gdzie szuka� bibliotek w wersji debug
# FIND_DIR_RELEASE Miejsca gdzie szuka� bibliotek w wersji release
# Wyja�nienie: extension u�ywany jest w sytuacji, gdy
# CMake nie potrafi wyszuka� biblioteki bez rozszerzenia (np. biblioteki stayczne na Unixie)
# w 99% przypadk�w jednak nic nie zawiera i w tych wypadkach rozszerzenia brane s� z suffixes.
# Rezultaty:
# 	${variable}_LIBRARY_DEBUG lokalizacja biblioteki w wersji debug
#   ${variable}_LIBRARY_RELEASE lokazliacja biblioteki w wersji release 
macro(FIND_LIBS_PATTERN variable releasePattern debugPattern extensions)
	
	#message("find libs: ${variable} ${releasePattern} ${debugPattern}")
	set(suffixes_copy ${CMAKE_FIND_LIBRARY_SUFFIXES})
	string(LENGTH "${extensions}" length)
	if (NOT length EQUAL 0)
		set(CMAKE_FIND_LIBRARY_SUFFIXES ${extensions})
	endif()
	
	if (FIND_DISABLE_CUSTOM_DIRECTORY)
		FIND_NOTIFY(${variable} "FIND_LIBS: only system directories!")
	endif()
	
	# wyszukanie wersji debug
	set(_lib_names)
	CREATE_NAMES_LIST("<?,lib>${debugPattern}${extensions}" _lib_names)
	
	FIND_NOTIFY(${variable} "FIND_LIBS: debug pattern ${debugPattern} unrolled to ${_lib_names}")
	if (NOT FIND_DISABLE_CUSTOM_DIRECTORY)
		# szukamy wersji release, najpierw w wyznaczonym miejscu
		find_library(${variable}_LIBRARY_DEBUG
			NAMES ${_lib_names}
			PATHS ${FIND_DIR_DEBUG}
			DOC "Location of release version of ${_lib_names}"
			NO_DEFAULT_PATH
		)
	endif()
	# potem w ca�ym systemie
	find_library(${variable}_LIBRARY_DEBUG
		NAMES ${_lib_names}
		DOC "Location of release version of ${_lib_names}"
	)
	
	
	# wyszukanie wersji release
	set(_lib_names)
	CREATE_NAMES_LIST("<?,lib>${releasePattern}${extensions}" _lib_names)
	
	FIND_NOTIFY(${variable} "FIND_LIBS: release pattern ${releasePattern} unrolled to ${_lib_names}")
	if (NOT FIND_DISABLE_CUSTOM_DIRECTORY)
		# szukamy wersji release, najpierw w wyznaczonym miejscu
		find_library(${variable}_LIBRARY_RELEASE
			NAMES ${_lib_names}
			PATHS ${FIND_DIR_RELEASE}
			DOC "Location of release version of ${_lib_names}"
			NO_DEFAULT_PATH
		)
	endif()
	# potem w ca�ym systemie
	find_library(${variable}_LIBRARY_RELEASE
		NAMES ${_lib_names}
		DOC "Location of release version of ${_lib_names}"
	)
	
	# przywracamy sufiksy
	set(CMAKE_FIND_LIBRARY_SUFFIXES ${suffixes_copy})
	
endmacro(FIND_LIBS_PATTERN)

###############################################################################

# Makro wyszukuje bibliotek statycznych lub plik�w lib dla wsp�dzielonych bibliotek (windows).
# Zak�ada, �e istniej� dwie zmienne:
# FIND_DIR_DEBUG Miejsca gdzie szuka� bibliotek w wersji debug
# FIND_DIR_RELEASE Miejsca gdzie szuka� bibliotek w wersji release
# Wyja�nienie: extension u�ywany jest w sytuacji, gdy
# CMake nie potrafi wyszuka� biblioteki bez rozszerzenia (np. biblioteki stayczne na Unixie)
# w 99% przypadk�w jednak nic nie zawiera i w tych wypadkach rozszerzenia brane s� z suffixes.
# Rezultaty:
# 	${variable}_LIBRARY_DEBUG lokalizacja biblioteki w wersji debug
#   ${variable}_LIBRARY_RELEASE lokazliacja biblioteki w wersji release 
macro(FIND_DLLS_PATTERN variable releasePattern debugPattern)
	
	if (FIND_DISABLE_CUSTOM_DIRECTORY)
		FIND_NOTIFY(${variable} "FIND_DLLS: only system directories!")
	endif()
	#message("find dlls: ${variable} ${releasePattern} ${debugPattern}")
	# wyszukanie wersji debug
	set(_lib_names)
	CREATE_NAMES_LIST("${debugPattern}.dll" _lib_names)
	# szukamy wersji debug
	FIND_NOTIFY(${variable} "FIND_DLLS: debug pattern ${debugPattern} unrolled to ${_lib_names}")
	if (NOT FIND_DISABLE_CUSTOM_DIRECTORY)
		find_file(${variable}_LIBRARY_DEBUG_DLL
			NAMES ${_lib_names}
			PATHS ${FIND_DIR_DEBUG}
			DOC "Location of debug version of ${variable}"
			NO_DEFAULT_PATH
		)
	endif()
	find_file(${variable}_LIBRARY_DEBUG_DLL
		NAMES ${_lib_names}
		DOC "Location of debug version of ${variable}"
	)
					
	# wyszukanie wersji release
	set(_lib_names)
	CREATE_NAMES_LIST("${releasePattern}.dll" _lib_names)
	# szukamy wersji release
	FIND_NOTIFY(${variable} "FIND_DLLS: release pattern ${releasePattern} unrolled to ${_lib_names}")
	if (NOT FIND_DISABLE_CUSTOM_DIRECTORY)
		find_file(${variable}_LIBRARY_RELEASE_DLL
			NAMES ${_lib_names}
			PATHS ${FIND_DIR_RELEASE}
			DOC "Location of release version of ${variable}"
			NO_DEFAULT_PATH
		)
	endif()
	find_file(${variable}_LIBRARY_RELEASE_DLL
		NAMES ${_lib_names}
		DOC "Location of release version of ${variable}"
	)

endmacro(FIND_DLLS_PATTERN)

###############################################################################

macro(FIND_EXECUTABLE variable pattern)
	FIND_NOTIFY(${variable} "FIND_EXECUTABLE: begin: ${${variable}}")
	if (FIND_DISABLE_CUSTOM_DIRECTORY)
		FIND_NOTIFY(${variable} "FIND_EXECUTABLE: only system directories!")
	endif()
	
	set(_lib_names)
	CREATE_NAMES_LIST("${pattern}" _lib_names)
	FIND_NOTIFY(${variable} "FIND_EXECUTABLE: pattern ${pattern} unrolled to ${_lib_names}")
	if (NOT FIND_DISABLE_CUSTOM_DIRECTORY)
		# najpierw przeszukiwany jest katalog release
		find_program(${variable}_EXECUTABLE
			NAMES ${_lib_names}
			PATHS ${FIND_DIR_RELEASE}
			DOC "Location of ${variable}"
			NO_DEFAULT_PATH
		)
		find_program(${variable}_EXECUTABLE
			NAMES ${_lib_names}
			PATHS ${FIND_DIR_DEBUG}
			DOC "Location of ${variable}"
			NO_DEFAULT_PATH
		)
	endif()
	find_program(${variable}_EXECUTABLE
		NAMES ${_lib_names}
		DOC "Location of ${variable}"
	)
	
	if (NOT ${variable}_EXECUTABLE)
		FIND_MESSAGE(WARNING "Static library ${variable} not found")
		FIND_NOTIFY_RESULT(0)
	else()
		set(${variable}_FOUND 1)
		FIND_NOTIFY_RESULT(1)
	endif()
	FIND_NOTIFY(${variable} "FIND_EXECUTABLE: end: ${${variable}_EXECUTABLE}")
endmacro(FIND_EXECUTABLE)

###############################################################################

# Makro wyszukuje biblioteki z pojedynczego pliku
# Zak�ada, �e istniej� dwie zmienne:
# FIND_DIR_DEBUG Miejsca gdzie szuka� bibliotek w wersji debug
# FIND_DIR_RELEASE Miejsca gdzie szuka� bibliotek w wersji release
# Rezultaty:
# 	${variable} Zaimportowana biblioteka
#   ${variable}_FOUND Flaga okre�laj�ca, czy si� uda�o
#   ${variable}_LIBRARY_DEBUG �cie�ka do biblioteki w wersji DEBUG.
#   ${variable}_LIBRARY_RELEASE �cie�ka do biblioteki w wersji RELEASE.
macro(ADD_LIBRARY_SINGLE variable names debugNames static)
	# szukamy lib�w
	if(${static})
		if ( WIN32 )
			FIND_LIBS_PATTERN(${variable} ${names} ${debugNames} ".lib")
		else()
			FIND_LIBS_PATTERN(${variable} ${names} ${debugNames} ".a")
		endif()
	#	# message("${names} ${static}")
	#	FIND_LIBS(${variable} "${names}" ".a;.lib" "")
	#	if (NOT (${variable}_LIBRARY_DEBUG OR ${variable}_LIBRARY_RELEASE) AND NOT CMAKE_HOST_WIN32)
	#		# czasem na linuxie nie wiedziec czemu wyszukiwanie bibliotek czasem nie dziala gdy nie ma rozszerzen
	#		FIND_LIBS(${variable} "${names}" ".a" ".a")
	#	endif()
	else()
		if ( WIN32 )
			FIND_LIBS_PATTERN(${variable} ${names} ${debugNames} ".dll")
		else()
			FIND_LIBS_PATTERN(${variable} ${names} ${debugNames} ".so")
		endif()
	#	FIND_LIBS(${variable} ${names} ".so;.dll" "")
	#	if (NOT (${variable}_LIBRARY_DEBUG OR ${variable}_LIBRARY_RELEASE) AND NOT CMAKE_HOST_WIN32)
	#		# czasem na linuxie nie wiedziec czemu wyszukiwanie bibliotek czasem nie dziala gdy nie ma rozszerzen
	#		FIND_LIBS(${variable} "${names}" ".so" ".so")
	#	endif()
	endif()

	# czy uda�o si� cokolwiek?
	if (${variable}_LIBRARY_DEBUG OR ${variable}_LIBRARY_RELEASE)
	
		# czy uda�o si� znale�� odpowiednie warianty?
		if ( ${variable}_LIBRARY_DEBUG AND ${variable}_LIBRARY_RELEASE )
			set(${variable} optimized ${${variable}_LIBRARY_RELEASE} debug ${${variable}_LIBRARY_DEBUG})			
		elseif ( ${variable}_LIBRARY_DEBUG )
			set(${variable} ${${variable}_LIBRARY_DEBUG})
			FIND_MESSAGE("Release version of ${variable} not found, using Debug version.")
		else()
			set(${variable} ${${variable}_LIBRARY_RELEASE})
			FIND_MESSAGE("Debug version of ${variable} not found, using Release version.")
		endif()
		
		# znale�li�my
		set(${variable}_FOUND 1)
		list( APPEND FIND_RESULTS ${variable})
		FIND_NOTIFY_RESULT(1)
	else()
		# nie znaleziono niczego
		if(${static})
			FIND_MESSAGE(WARNING "Static library ${variable} not found")
		else()
			FIND_MESSAGE(WARNING "Shared Library ${variable} not found")
		endif()
		FIND_NOTIFY_RESULT(0)
	endif()
	
endmacro (ADD_LIBRARY_SINGLE)


###############################################################################

macro(FIND_STATIC_EXT variable names debugNames)
	FIND_NOTIFY(${variable} "FIND_STATIC_EXT: begin: ${${variable}}")
	ADD_LIBRARY_SINGLE(${variable} ${names} ${debugNames} 1)
	FIND_NOTIFY(${variable} "FIND_STATIC_EXT: libs: ${${variable}}")
endmacro(FIND_STATIC_EXT)

# Wyszukuje bibliotek� statyczn�
# variable	Nazwa zmiennej
# shortname	Nazwa biblioteki (nazwa pliku)
# Odno�nie rezulat�w przeczytaj komentarz do makra ADD_LIBRARY_SINGLE
macro(FIND_STATIC variable names)
	FIND_STATIC_EXT(${variable} ${names} "${names}<d,?>")
endmacro(FIND_STATIC)

###############################################################################

macro (FIND_SHARED_EXT variable names debugNames dllNames dllDebugNames)
	FIND_NOTIFY(${variable} "FIND_SHARED_EXT: begin: ${${variable}}")
	if (NOT WIN32)
		# jeden plik
		ADD_LIBRARY_SINGLE(${variable} ${names} ${debugNames} 0)
	else()
		# b�dzie plik lib i dll...
		# szukamy lib�w
		FIND_LIBS_PATTERN(${variable} ${names} ${debugNames} ".lib")
		# szukamy dllek
		FIND_DLLS_PATTERN(${variable} "${dllNames}" "${dllDebugNames}")
		set(MESSAGE_BODY "${variable} (${dllNames})")
		
		if ((${variable}_LIBRARY_DEBUG AND ${variable}_LIBRARY_DEBUG_DLL) OR (${variable}_LIBRARY_RELEASE AND ${variable}_LIBRARY_RELEASE_DLL))
			
			# ok, mamy co najmniej jedn� wersj�
			if ((${variable}_LIBRARY_DEBUG AND ${variable}_LIBRARY_DEBUG_DLL) AND
				(${variable}_LIBRARY_RELEASE AND ${variable}_LIBRARY_RELEASE_DLL))
				set(${variable} optimized ${${variable}_LIBRARY_RELEASE} debug ${${variable}_LIBRARY_DEBUG})
			elseif (${variable}_LIBRARY_DEBUG AND ${variable}_LIBRARY_DEBUG_DLL)
				set(${variable} ${${variable}_LIBRARY_DEBUG})
				set(${variable}_LIBRARY_RELEASE_DLL ${${variable}_LIBRARY_DEBUG_DLL})
				FIND_MESSAGE("Release version of ${MESSAGE_BODY} not found, using Debug version.")
			else()
				set(${variable} ${${variable}_LIBRARY_RELEASE})
				set(${variable}_LIBRARY_DEBUG_DLL ${${variable}_LIBRARY_RELEASE_DLL})
				FIND_MESSAGE("Debug version of ${MESSAGE_BODY} not found, using Release version.")
			endif()
		
			# znale�li�my
			set(${variable}_FOUND 1)
			list( APPEND FIND_RESULTS ${variable})
			
			# dodajemy do list do skopiowania
			list( APPEND FIND_MODULES_TO_COPY_DEBUG ${${variable}_LIBRARY_DEBUG_DLL} )
			list( APPEND FIND_MODULES_TO_COPY_RELEASE ${${variable}_LIBRARY_RELEASE_DLL} )
			FIND_NOTIFY_RESULT(1)
		else()
			# nie znaleziono niczego
			FIND_MESSAGE(WARNING "Shared library${MESSAGE_BODY} was not found")
			FIND_NOTIFY_RESULT(0)
		endif()
	endif()
	FIND_NOTIFY(${variable} "FIND_SHARED_EXT: libs: ${${variable}}; debug dll: ${${variable}_LIBRARY_DEBUG}; release dll: ${${variable}_LIBRARY_RELEASE}")
endmacro( FIND_SHARED_EXT )

# Wyszukuje bibliotek� statyczn�
# variable	Nazwa zmiennej
# shortname	Nazwa biblioteki (nazwa pliku) .so dla Unixa lub .lib dla Windowsa
# ... Mo�liwe nazwy biblioteki .dll dla Windowsa.
# Odno�nie rezulat�w przeczytaj komentarz do makra ADD_LIBRARY_SINGLE
macro (FIND_SHARED variable names dllNames)
	FIND_SHARED_EXT(${variable} ${names} "${names}<d,?>" ${dllNames} "${dllNames}<d,?>")
endmacro (FIND_SHARED)

###############################################################################

macro (FIND_MODULE_EXT variable isSystemModule names debugNames)
	FIND_NOTIFY(${variable} "FIND_MODULE_EXT: begin: ${${variable}}")
	# czy wyszukujemy tylko w �cie�ce systemowej?
	if (${isSystemModule} STREQUAL "TRUE")
		set(FIND_DISABLE_CUSTOM_DIRECTORY ON)
	endif()
	
	# na Unixie po prostu dodajemy bibliotek� wsp�dzielon�
	ADD_LIBRARY_SINGLE(${variable} "${names}" "${debugNames}" 0)
	# jezeli znaleziono to trzeba usunac z listy modulow
	if (${variable}_FOUND)
		list( REMOVE_ITEM FIND_RESULTS ${variable})
		# je�li to nie modu� systemowy dodajemy do listy
		if (${isSystemModule} STREQUAL "FALSE")
			FIND_NOTIFY(${variable} "FIND_MODULE_EXT: will copy; debug dll: ${${variable}_LIBRARY_DEBUG}; release dll: ${${variable}_LIBRARY_RELEASE}")
			list( APPEND FIND_MODULES_TO_COPY_DEBUG ${${variable}_LIBRARY_DEBUG} )
			list( APPEND FIND_MODULES_TO_COPY_RELEASE ${${variable}_LIBRARY_RELEASE} )
		else()
			FIND_NOTIFY(${variable} "FIND_MODULE_EXT: won't copy; debug dll: ${${variable}_LIBRARY_DEBUG}; release dll: ${${variable}_LIBRARY_RELEASE}")
		endif()
	endif()
		
	# czy wyszukujemy tylko w �cie�ce systemowej?
	if (${isSystemModule} STREQUAL "TRUE")
		set(FIND_DISABLE_CUSTOM_DIRECTORY)
		unset(${variable}_LIBRARY_DEBUG CACHE )
		unset(${variable}_LIBRARY_RELEASE CACHE )
		unset(${variable}_LIBRARY_DEBUG_DLL CACHE )
		unset(${variable}_LIBRARY_RELEASE_DLL CACHE )
		unset(${variable}_LIBRARY_DIR_DEBUG CACHE )
	    unset(${variable}_LIBRARY_DIR_RELEASE CACHE )
	endif()	
	FIND_NOTIFY(${variable} "FIND_MODULE_EXT: libs: ${${variable}}")
endmacro(FIND_MODULE_EXT)

# Wyszukuje bibliotek� statyczn�
# variable	Nazwa zmiennej
# shortname	Nazwa biblioteki (nazwa pliku) .so dla Unixa lub .lib dla Windowsa
# ... Mo�liwe nazwy biblioteki .dll dla Windowsa.
# Odno�nie rezulat�w przeczytaj komentarz do makra ADD_LIBRARY_SINGLE
macro (FIND_MODULE variable isSystemModule names)
	FIND_MODULE_EXT(${variable} ${isSystemModule} ${names} "${names}<d,?>")
endmacro (FIND_MODULE)
###############################################################################

macro(FIND_MESSAGE ...)
	if (NOT FIND_SUPRESS_MESSAGES)
		message(${ARGV})
	endif()
endmacro(FIND_MESSAGE)

###############################################################################

macro(FIND_NOTIFY_RESULT value)
	if ( ${value} ) 
		if ( NOT FIND_RESULTS_LOGICAL_OR )
			set(FIND_RESULTS_LOGICAL_OR 1)
		endif()
	else()
		if ( FIND_RESULTS_LOGICAL_AND )
			set(FIND_RESULTS_LOGICAL_AND 0)
		endif()
	endif()
endmacro(FIND_NOTIFY_RESULT)

###############################################################################

macro(FIND_COPY_AND_INSTALL_MODULES buildType subDir)

	# wybieramy odpowiednia liste
	string(TOUPPER "${buildType}" buildTypeUpper)
	if ("${buildTypeUpper}" STREQUAL "DEBUG")
		set(MODULES_LIST ${FIND_MODULES_TO_COPY_DEBUG})
	else()
		set(MODULES_LIST ${FIND_MODULES_TO_COPY_RELEASE})
	endif()
	
	# kopiujemy modul
	foreach (module ${MODULES_LIST})
	
		get_filename_component(moduleNameWE ${module} NAME_WE)
		get_filename_component(moduleName ${module} NAME)
		
		#message("${moduleName}")
		
		# czy zdefiniowano sufix dla tego modu�u?
		if (FIND_MODULE_PREFIX_${moduleNameWE})
			set(moduleName ${FIND_MODULE_PREFIX_${moduleNameWE}}${moduleName})
		endif()
		if ("${subDir}" STREQUAL "")
			# message("configure_file(${module} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${moduleName} COPYONLY)")
			configure_file(${module} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${moduleName} COPYONLY)
			message(STATUS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${moduleName} <- ${module}")
		else()
			# message("configure_file(${module} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${subDir}/${moduleName} COPYONLY)")
			configure_file(${module} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${subDir}/${moduleName} COPYONLY ESCAPE_QUOTES)
			message(STATUS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${subDir}/${moduleName} <- ${module}")
		endif()
		# instalacja pliku
		install(FILES ${module} DESTINATION bin/${FIND_MODULE_PREFIX_${moduleNameWE}} CONFIGURATIONS ${buildType})
	endforeach()

endmacro(FIND_COPY_AND_INSTALL_MODULES)

###############################################################################

# Tworzy list� nazw na podstawie wzoru; miejsca podmiany musz� by� w ostrych
# nawiasach, natomiast warto�ci oddzielone przecinkiem; znak "?" to specjalna
# warto�� oznaczaj�ca pusty �a�cuch
# nie mog� powtarza� si� identyczne miejsca podmiany! (u�omno�� CMake)
# przyk�ad: pattern = bib<1,2,3>v<?,_d>
#			result = bib1v;bib1v_d;bib2v;bib2v_d;bib3v;bib3v_d 
macro (CREATE_NAMES_LIST pattern result)
	set(_names ${pattern})
	set(_pattern ${pattern})
	foreach( id RANGE 5 )
		# pobranie opcji
		string(REGEX MATCH "<([^<]*)>" _toReplace ${_pattern})
		if( _toReplace ) 
			# konwersja na list�
			if (NOT CMAKE_MATCH_1 STREQUAL "")
				string(REPLACE "," ";" _options ${CMAKE_MATCH_1})
			else()
				set(_options "?")
			endif()
			# usuni�cie opcji z �a�cucha
			string(REPLACE ${_toReplace} "X" _replaced ${_pattern})
			set(_pattern ${_replaced})
			# podmiana klucza
			set(_newNames)
			foreach( comb ${_names} )
				foreach (opt ${_options})
					# znak zapytania traktowany jako pusty znak
					if (opt STREQUAL "?")
						string(REPLACE ${_toReplace} "" _temp ${comb})
					else()
						string(REPLACE ${_toReplace} ${opt} _temp ${comb})
					endif()
					list(APPEND _newNames ${_temp})
				endforeach()
			endforeach()
			set(_names ${_newNames})
			#message("iter ${id}: ${_newNames}")
		endif()
	endforeach()
	set(${result} ${_names})
endmacro (CREATE_NAMES_LIST)

###############################################################################

macro(FIND_NOTIFY var msg)
	if (FIND_VERBOSE)
		message(STATUS "FIND>${var}>${msg}")
	endif()
endmacro()

###############################################################################
