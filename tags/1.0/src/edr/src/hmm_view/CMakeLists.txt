BEGIN_PROJECT("executable")

SET_PRIVATE_HEADERS(
	IAppUsageContext.h
	IAppUsageContextManager.h
	AboutDialog.h
	HmmMainWindow.h
	TreeItemHelper.h
	#EMGFilter.h
	DataFilterWidget.h
	FilterEntryWidget.h
	FilterCommand.h
	TreeBuilder.h
	Vector3DFilterCommand.h
	#ConfigurationDialog.h
	#ConfigurationPainter.h
	#ConfigurationPainterAreas.h
	AnalisisTreeWidget.h
	AnalisisWidget.h
	IllnessUnit.h
	SummaryWindow.h
	HmmContexts.h
	Measurements.h
	MultiserieColorStrategy.h
	TreeRefresher.h
	ContextAction.h
	ContextEventFilter.h
	HmmVdf.h
	HmmSources.h
	HmmSinks.h
	hmmPCH.h
)

SET_SOURCE_FILES (
	AboutDialog.cpp
	main.cpp
	HmmMainWindow.cpp
	TreeItemHelper.cpp
	#EMGFilter.cpp
	DataFilterWidget.cpp
	FilterCommand.cpp
	FilterEntryWidget.cpp
	TreeBuilder.cpp
	#ConfigurationDialog.cpp
	#ConfigurationPainter.cpp
	#ConfigurationPainterAreas.cpp
	AnalisisTreeWidget.cpp
	IllnessUnit.cpp
	AnalisisWidget.cpp
	HmmContexts.cpp
	SummaryWindow.cpp
	TreeRefresher.cpp
	Measurements.cpp
	ContextEventFilter.cpp
	Vector3DFilterCommand.cpp
	hmmPCH.cpp
	HmmVdf.cpp
	HmmSources.cpp
	HmmSinks.cpp
)

SET_MOC_FILES(
	AboutDialog.h
	ContextEventFilter.h
	ContextAction.h
    HmmMainWindow.h
	DataFilterWidget.h
	AnalisisTreeWidget.h
	FilterEntryWidget.h
	Vector3DFilterCommand.h
	#ConfigurationPainter.h
	#ConfigurationDialog.h
	AnalisisWidget.h
	HmmContexts.h
	SummaryWindow.h
)

SET_UI_FILES(
	AboutDialog.ui
	PseudoLoginWidget.ui
	toolboxmaindeffile.ui
	filter.ui
	filterEntry.ui
	#configurationDialog.ui
	AnalisisWidget.ui
	#FilterConfiguratorWidget.ui
)

SET_RC_FILES (
	hmmViewIcons.qrc
)

SET_PRECOMPILED_HEADER(hmmPCH.h hmmPCH.cpp)

GENERATE_PROJECT_TRANSLATIONS()

#------------------------------------------------------------------------------
# DEFINICJE WYNIKAJ�CE Z KONFLIKTU QT - BOOST (foreach)
# wy�aczamy s�owa kluczowe qt, zast�pujemy je makrami
# u�ywamy tylko i wy��cznie BOOST_FOREACH!!
add_definitions(-DQT_NO_KEYWORDS -Dsignals=Q_SIGNALS -Dslots=Q_SLOTS -Demit=Q_EMIT)

END_PROJECT()