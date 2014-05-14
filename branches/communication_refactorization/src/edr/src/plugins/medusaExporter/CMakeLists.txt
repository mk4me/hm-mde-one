BEGIN_PROJECT("dynamic")


SET_PRIVATE_HEADERS (
	AnnotationData.h
	CSVExporter.h
	ExporterModel.h
	IExporter.h
	INIExporter.h
	MedusaExporterPCH.h
	MedusaExporterService.h
	MedusaExporterServiceWidget.h
	PythonExporter.h
	UserAnnotationUpdater.h
)

SET_SOURCE_FILES(
	AnnotationData.cpp
	CSVExporter.cpp
	ExporterModel.cpp
	INIExporter.cpp
	MedusaExporterPCH.cpp
	MedusaExporterPlugin.cpp
	MedusaExporterService.cpp
	MedusaExporterServiceWidget.cpp
	PythonExporter.cpp
	UserAnnotationUpdater.cpp
)

SET_MOC_FILES(
	MedusaExporterServiceWidget.h
)
SET_UI_FILES(
	medusaExporter.ui
)

SET_RC_FILES (
	medusaExporterRC.qrc
)
#------------------------------------------------------------------------------
# prekompilowany nag³ówek
SET_PRECOMPILED_HEADER( MedusaExporterPCH.h MedusaExporterPCH.cpp)

END_PROJECT()