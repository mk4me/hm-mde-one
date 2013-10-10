BEGIN_PROJECT("dynamic")

SET_PUBLIC_HEADERS (
	Export.h
	Dicom.h
)

SET_PRIVATE_HEADERS (
	DicomParser.h
	DicomPCH.h
	DicomVisualizer.h
	DicomSource.h
	DicomSourceWidget.h
)

SET_SOURCE_FILES(
	DicomPlugin.cpp
	DicomParser.cpp
	DicomVisualizer.cpp
	DicomSource.cpp
	DicomSourceWidget.cpp
	DicomPCH.cpp
)

SET_MOC_FILES(
	DicomVisualizer.h
	DicomSourceWidget.h
)
#------------------------------------------------------------------------------
# prekompilowany nag³ówek
SET_PRECOMPILED_HEADER( C3DPCH.h C3DPCH.cpp)

END_PROJECT()