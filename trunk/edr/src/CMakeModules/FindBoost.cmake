# przygotowanie do szukania
FIND_INIT(BOOST boost)

# szukanie
FIND_STATIC_EXT(BOOST_REGEX "libboost_regex<-mt,>" "libboost_regex<-mt,><-gd,-d>")
FIND_STATIC_EXT(BOOST_SYSTEM "libboost_system<-mt,>" "libboost_system<-mt,><-gd,-d>")
FIND_STATIC_EXT(BOOST_FILESYSTEM "libboost_filesystem<-mt,>" "libboost_filesystem<-mt,><-gd,-d>")
# koniec
FIND_FINISH(BOOST)

# Wy³¹czamy automatyczne linkowanie boosta
add_definitions(-DBOOST_ALL_NO_LIB)

# sprawdzenie
if (BOOST_REGEX_FOUND AND BOOST_SYSTEM_FOUND AND BOOST_FILESYSTEM_FOUND)
	set(BOOST_FOUND 1)

endif()