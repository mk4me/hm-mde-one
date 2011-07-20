# przygotowanie do szukania
FIND_INIT(BOOST boost)

# szukanie
FIND_STATIC_EXT(BOOST_REGEX "boost_regex<-mt,>" "boost_regex<-mt,><-gd,-d>")
FIND_STATIC_EXT(BOOST_SYSTEM "boost_system<-mt,>" "boost_system<-mt,><-gd,-d>")
FIND_STATIC_EXT(BOOST_FILESYSTEM "boost_filesystem<-mt,>" "boost_filesystem<-mt,><-gd,-d>")
FIND_STATIC_EXT(BOOST_PROGRAM_OPTIONS "boost_program_options<-mt,>" "boost_program_options<-mt,><-gd,-d>")
# koniec
FIND_FINISH(BOOST)

# Wy��czamy automatyczne linkowanie boosta
add_definitions(-DBOOST_ALL_NO_LIB)

# sprawdzenie
if (BOOST_REGEX_FOUND AND BOOST_SYSTEM_FOUND AND BOOST_FILESYSTEM_FOUND AND BOOST_PROGRAM_OPTIONS)
	set(BOOST_FOUND 1)

endif()