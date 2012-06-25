project(hmm_view)
# nazwa
set(TARGET_TARGETNAME
	hmm_view
)

# UWAGA: poniewa¿ u¿ywamy "source_group", ka¿dy nag³ówek/Ÿród³o musi do którejœ
# trafiæ; w przeciwnym wypadku CMake generuje b³êdny projekt

#------------------------------------------------------------------------------

# nag³ówki prywatne
set(PRIVATE_H
	AboutDialog.h
	HmmMainWindow.h
	EDRDockWidgetSet.h
	EDRDockWidgetManager.h
	TreeItemHelper.h
	EmgFilter.h
	DataFilterWidget.h
	FilterEntryWidget.h
	FilterCommand.h
	TreeBuilder.h
	Vector3DFilterCommand.h
	ConfigurationDialog.h
	ConfigurationPainter.h
	ConfigurationPainterAreas.h
	AnalisisTreeWidget.h
	AnalisisWidget.h
	IllnessUnit.h
	SummaryWindow.h
	HmmContexts.h
	TextEdit.h
	Measurements.h
	MultiserieColorStrategy.h
	TreeRefresher.h
	ContextAction.h
	ContextEventFilter.h
)
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRIVATE_H})

# nag³ówki publiczne
string(REPLACE ${PROJECT_ROOT} ${PROJECT_INCLUDE_ROOT} HEADER_PATH ${PROJECT_SOURCE_DIR})
set(PUBLIC_H
	#${HEADER_PATH}/SubjectSystemConfiguration.h
)
source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES ${PUBLIC_H})
set(TARGET_H ${PRIVATE_H} ${PUBLIC_H})

#------------------------------------------------------------------------------

# Ÿród³a
set(TARGET_SRC
	AboutDialog.cpp
	main.cpp
	HmmMainWindow.cpp
	EDRDockWidgetSet.cpp
	EDRDockWidgetManager.cpp
	TreeItemHelper.cpp
	EmgFilter.cpp
	DataFilterWidget.cpp
	FilterCommand.cpp
	FilterEntryWidget.cpp
	TreeBuilder.cpp
	ConfigurationDialog.cpp
	ConfigurationPainter.cpp
	ConfigurationPainterAreas.cpp
	AnalisisTreeWidget.cpp
	IllnessUnit.cpp
	AnalisisWidget.cpp
	HmmContexts.cpp
	SummaryWindow.cpp
	TextEdit.cpp
	TreeRefresher.cpp
	Measurements.cpp
	ContextEventFilter.cpp
	Vector3DFilterCommand.cpp
)
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_SRC})

# metadane generowane przez MOC
SET(TARGET_MOC_H
	AboutDialog.h
	ContextEventFilter.h
	PseudoLoginWidget.h
	ContextAction.h
    HmmMainWindow.h
	EDRDockWidgetSet.h
	EDRDockWidgetManager.h
	DataFilterWidget.h
	AnalisisTreeWidget.h
	FilterEntryWidget.h
	Vector3DFilterCommand.h
	ConfigurationPainter.h
	ConfigurationDialog.h
	AnalisisWidget.h
	TextEdit.h
	HmmContexts.h
	SummaryWindow.h
)
QT4_WRAP_CPP(TARGET_MOC_SRC ${TARGET_MOC_H})
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_MOC_SRC})
set(TARGET_SRC ${TARGET_SRC} ${TARGET_MOC_SRC})

#------------------------------------------------------------------------------
# UI
set(TARGET_UI
	ui/AboutDialog.ui
	ui/PseudoLoginWidget.ui
	ui/toolboxmaindeffile.ui
	ui/filter.ui
	ui/filterEntry.ui
	ui/configurationDialog.ui
	ui/AnalisisWidget.ui
	ui/FilterConfiguratorWidget.ui
)
QT4_WRAP_UI(TARGET_UI_H ${TARGET_UI})
source_group("${SOURCEGROUP_UI}" FILES ${TARGET_UI_H} ${TARGET_UI})
set(TARGET_H ${TARGET_H} ${TARGET_UI_H} ${TARGET_UI})

#------------------------------------------------------------------------------
# RC
set(TARGET_RC
	ui/Icons.qrc
)
QT4_ADD_RESOURCES(TARGET_RCC_SRC ${TARGET_RC})
source_group("${SOURCEGROUP_UI}" FILES ${TARGET_RC} ${TARGET_RCC_SRC})
set(TARGET_SRC ${TARGET_SRC} ${TARGET_RCC_SRC} ${TARGET_RC})

#------------------------------------------------------------------------------
# prekompilowany nag³ówek
set(PRECOMPILED_H HmmPCH.h)
set(PRECOMPILED_SRC HmmPCH.cpp)
SET_PRECOMPILED_HEADER(${PRECOMPILED_H} ${PRECOMPILED_SRC} "${TARGET_SRC}")
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRECOMPILED_H})
set(TARGET_H ${TARGET_H} ${PRECOMPILED_H})
source_group("${SOURCEGROUP_SOURCES}" FILES ${PRECOMPILED_SRC})
set(TARGET_SRC ${TARGET_SRC} ${PRECOMPILED_SRC})



#------------------------------------------------------------------------------
# DEFINICJE WYNIKAJ¥CE Z KONFLIKTU QT - BOOST (foreach)
# wy³anczamy s³owa kluczowe qt, zastêpujemy je makrami
# u¿ywamy tylko i wy³¹cznie BOOST_FOREACH!!
add_definitions(-DQT_NO_KEYWORDS -Dsignals=Q_SIGNALS -Dslots=Q_SLOTS -Demit=Q_EMIT)

#------------------------------------------------------------------------------
# tworzenie

# obsluga OpenGL
set(QT_USE_QTOPENGL TRUE) 
include(${QT_USE_FILE})

# kompilacja do exe
option(EDR_WIN32_ENABLE_CONSOLE "Enable console on Win32?" OFF)
if (EDR_WIN32_ENABLE_CONSOLE)
	add_executable(${TARGET_TARGETNAME} ${TARGET_SRC} ${TARGET_H})
else()
	add_executable(${TARGET_TARGETNAME} WIN32 ${TARGET_SRC} ${TARGET_H})
endif()

#instalacja
install(TARGETS ${TARGET_TARGETNAME} RUNTIME DESTINATION bin COMPONENT ${PROJECT_NAME})
install(TARGETS ${TARGET_TARGETNAME} LIBRARY DESTINATION lib/${PROJECT_NAME} ARCHIVE DESTINATION lib/${PROJECT_NAME})
install(FILES ${PUBLIC_H} DESTINATION include/${TARGET_TARGETNAME})
# nag³ówki
set(CORE_PRIVATES ${PROJECT_ROOT}/core)
list(APPEND DEFAULT_PROJECT_INCLUDES ${CORE_PRIVATES})
include_directories(${DEFAULT_PROJECT_INCLUDES})
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	

ON_PROJECT_ADDED(hmm_view)
