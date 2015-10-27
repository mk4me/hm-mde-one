BEGIN_PROJECT("executable")

SET_PRIVATE_HEADERS(
	DicomWatcher.h
	WatcherConfig.h
)

SET_MOC_FILES(
	DicomWatcher.h
)

SET_SOURCE_FILES (
	DicomWatcher.cpp
	WatcherConfig.cpp
	main.cpp
)

END_PROJECT()
