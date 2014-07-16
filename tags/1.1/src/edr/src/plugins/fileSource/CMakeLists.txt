BEGIN_PROJECT("dynamic")


SET_PRIVATE_HEADERS (
	FileSource.h
	FileSourceWidget.h
)

SET_SOURCE_FILES(
	FileSource.cpp
	FileSourceWidget.cpp
	FileSourcePlugin.cpp
)
SET_MOC_FILES (
	FileSourceWidget.h
)
END_PROJECT()