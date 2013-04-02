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
	ThreadTest.cpp
	ThreadPoolTest.cpp
	ThreadGroupTest.cpp
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
	ThreadTest.h
	ThreadPoolTest.h
	ThreadGroupTest.h
)

END_PROJECT()