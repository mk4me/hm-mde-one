BEGIN_PROJECT("dynamic")

SET_PUBLIC_HEADERS (
	Export.h
	Dicom.h
	ILayer.h
	ILayeredImage.h
)

SET_PRIVATE_HEADERS (
	DicomParser.h
	DicomPCH.h
	DicomVisualizer.h
	DicomSource.h
	DicomSourceWidget.h
	BackgroundLayer.h
	LayeredImage.h
	LayeredImageVisualizer.h
	LayeredImageVisualizerView.h
)

SET_SOURCE_FILES(
	DicomPlugin.cpp
	DicomParser.cpp
	DicomVisualizer.cpp
	DicomSource.cpp
	DicomSourceWidget.cpp
	BackgroundLayer.cpp
	LayeredImage.cpp
	LayeredImageVisualizer.cpp
	LayeredImageVisualizerView.cpp
	DicomPCH.cpp
)

SET_MOC_FILES(
	LayeredImageVisualizer.h
	DicomVisualizer.h
	DicomSourceWidget.h
	LayeredImageVisualizerView.h
)
SET_UI_FILES(
	layeredImageVisualizer.ui
	dicomSource.ui
)

SET_RC_FILES (
	dicomRC.qrc
)
#------------------------------------------------------------------------------
# prekompilowany nag³ówek
SET_PRECOMPILED_HEADER( DicomPCH.h DicomPCH.cpp)

END_PROJECT()