BEGIN_PROJECT("executable")

SET_SOURCE_FILES( 
	test.cpp
	AlignTest.cpp
	PoolTest.cpp
	UtilsTest.cpp
	EnumTest.cpp
	ObserverPatternTest.cpp
	NamedTreeBaseTest.cpp
	SynchronizedTest.cpp
	DataChannelTest.cpp
)

SET_PRIVATE_HEADERS(
	AlignTest.h
	PoolTest.h
	UtilsTest.h
	EnumTest.h
	ObserverPatternTest.h
	NamedTreeBaseTest.h
	SynchronizedTest.h
	DataChannelTest.h
)

END_PROJECT()