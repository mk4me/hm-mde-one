# przygotowanie do szukania
FIND_INIT(BOOST boost)

# funkcja wykrywaj¹ce wersjê BOOST
function(BOOST_FIND_VERSION path)    
	# inicjalizacja
	set(BOOST_VERSION "Unknown" CACHE STRING "Unknown version")
	# próba odczytania wersji z pliku
	if (EXISTS ${path})
		file(READ "${path}" _boost_Version_contents)
		string(REGEX REPLACE ".*#define.*BOOST_LIB_VERSION.*([0-9]+_[0-9]+).*"
            "\\1" _boost_version ${_boost_Version_contents})
		if (BOOST_VERSION STREQUAL "Unknown")
			set(BOOST_VERSION "${_boost_version}" CACHE STRING "The version of BOOST_VERSION${suffix} which was detected" FORCE)
		endif()
    endif()
	# czy siê uda³o?
	if (BOOST_VERSION STREQUAL "Unknown")
		message("Unknown version of BOOST_VERSION. File ${path} could not be read. This may result in further errors.")
	endif()
endfunction(BOOST_FIND_VERSION)

# wykrycie wersji osg
BOOST_FIND_VERSION("${BOOST_INCLUDE_DIR}/boost/version.hpp")


set(boost_ver "-${BOOST_VERSION}")

#if ( WIN32 )		
	set(boost_cmpl "-vc100")
	# szukanie
	#FIND_SHARED(BOOST_SYSTEM "boost_system<-mt,>" "boost_system<-mt,><-gd,-d>")
	FIND_SHARED(BOOST_SYSTEM "boost_system<${boost_cmpl},?><-mt,?><-gd,?><${boost_ver},?>" "boost_system<${boost_cmpl},?><-mt,?><-gd,?><${boost_ver},?>")
	FIND_SHARED(BOOST_FILESYSTEM "boost_filesystem<${boost_cmpl},?><-mt,?><-gd,?><${boost_ver},?>" "boost_filesystem<${boost_cmpl},?><-mt,?><-gd,?><${boost_ver},?>")
	FIND_SHARED(BOOST_PROGRAM_OPTIONS "boost_program_options<${boost_cmpl},?><-mt,?><-gd,?><${boost_ver},?>" "boost_program_options<${boost_cmpl},?><-mt,?><-gd,?><${boost_ver},?>")
	FIND_SHARED(BOOST_SERIALIZATION "boost_serialization<${boost_cmpl},?><-mt,?><-gd,?><${boost_ver},?>" "boost_serialization<${boost_cmpl},?><-mt,?><-gd,?><${boost_ver},?>")
	FIND_SHARED(BOOST_DATE_TIME "boost_date_time<${boost_cmpl},?><-mt,?><-gd,?><${boost_ver},?>" "boost_date_time<${boost_cmpl},?><-mt,?><-gd,?><${boost_ver},?>")
	FIND_SHARED(BOOST_TIMER "boost_timer<${boost_cmpl},?><-mt,?><-gd,?><${boost_ver},?>" "boost_timer<${boost_cmpl},?><-mt,?><-gd,?><${boost_ver},?>")
	FIND_SHARED(BOOST_CHRONO "boost_chrono<${boost_cmpl},?><-mt,?><-gd,?><${boost_ver},?>" "boost_chrono<${boost_cmpl},?><-mt,?><-gd,?><${boost_ver},?>")
#else()
#	message("WSZEDL !!!!!!!!!!")
#	FIND_STATIC(BOOST_SYSTEM "libboost_system")
#	FIND_STATIC(BOOST_FILESYSTEM "libboost_filesystem")
#	FIND_STATIC(BOOST_PROGRAM_OPTIONS "libboost_program_options")
#	FIND_STATIC(BOOST_SERIALIZATION "libboost_serialization")
#	FIND_STATIC(BOOST_DATE_TIME "libboost_date_time")
#	FIND_STATIC(BOOST_TIMER "libboost_timer")
#	FIND_STATIC(BOOST_CHRONO "libboost_chrono")
#endif()
# koniec
FIND_FINISH(BOOST)

# Wy³¹czamy automatyczne linkowanie boosta
set(BOOST_CUSTOM_COMPILER_DEFINITIONS BOOST_ALL_NO_LIB BOOST_PROGRAM_OPTIONS_DYN_LINK)
#add_definitions(-DBOOST_ALL_NO_LIB)
#add_definitions(-DBOOST_PROGRAM_OPTIONS_DYN_LINK)

# sprawdzenie
if (BOOST_SYSTEM_FOUND AND
	BOOST_FILESYSTEM_FOUND AND
	BOOST_PROGRAM_OPTIONS AND
	BOOST_SERIALIZATION AND
	BOOST_DATE_TIME AND
	BOOST_CHRONO AND
	BOOST_TIMER)
 set(BOOST_FOUND 1)

endif()
