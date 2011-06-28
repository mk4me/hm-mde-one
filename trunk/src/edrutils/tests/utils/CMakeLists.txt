# nazwa
set(TARGET_TARGETNAME utils_test)

# Ÿród³a
set(TARGET_SRC 
	test.cpp
	AlignTest.cpp
	PoolTest.cpp
	UtilsTest.cpp
	EnumTest.cpp
	ObserverPatternTest.cpp
	NamedTreeBaseTest.cpp
	SynchronizedTest.cpp
)

# nag³ówki
set(TARGET_H
	AlignTest.h
	PoolTest.h
	UtilsTest.h
	EnumTest.h
	ObserverPatternTest.h
	NamedTreeBaseTest.h
	SynchronizedTest.h
)

# kompilacja do wspó³dzielonej biblioteki
add_executable(${TARGET_TARGETNAME} ${TARGET_SRC} ${TARGET_H})
# nag³ówki
include_directories( ${DEFAULT_PROJECT_INCLUDES} )
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	
# labelka
set_target_properties(${TARGET_TARGETNAME} PROPERTIES PROJECT_LABEL "Test of utils")	