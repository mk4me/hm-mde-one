BEGIN_PROJECT("dynamic")

SET_PUBLIC_HEADERS(
	#IllnessUnit.h
	EMGFilter.h
	ConfigurationDialog.h
	ConfigurationPainter.h
	ConfigurationPainterAreas.h
	Export.h
)

SET_SOURCE_FILES(
	EMGFilter.cpp
	#IllnessUnit.cpp
	HmmLibPCH.cpp
	ConfigurationDialog.cpp
	ConfigurationPainter.cpp
	ConfigurationPainterAreas.cpp
)

SET_PRIVATE_HEADERS(
	HmmLibPCH.h
)

SET_UI_FILES(
	configurationDialog.ui
	FilterConfiguratorWidget.ui
)

SET_MOC_FILES(
	ConfigurationDialog.h
	ConfigurationPainter.h
)

SET_PRECOMPILED_HEADER (HmmLibPCH.h HmmLibPCH.cpp)

END_PROJECT()
