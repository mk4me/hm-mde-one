BEGIN_PROJECT("dynamic")

SET_PUBLIC_HEADERS (
	IMedusaExporterService.h
)

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
# prekompilowany nag��wek
SET_PRECOMPILED_HEADER( MedusaExporterPCH.h MedusaExporterPCH.cpp)

#------------------------------------------------------------------------------
# DEFINICJE WYNIKAJ�CE Z KONFLIKTU QT - BOOST (foreach)
# wy�aczamy s�owa kluczowe qt, zast�pujemy je makrami
# u�ywamy tylko i wy��cznie BOOST_FOREACH!!
add_definitions(-DQT_NO_KEYWORDS -Dsignals=Q_SIGNALS -Dslots=Q_SLOTS -Demit=Q_EMIT)

END_PROJECT()