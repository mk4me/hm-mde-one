project(corelib)
# nazwa
set(TARGET_TARGETNAME
	corelib
)

# UWAGA: poniewa¿ u¿ywamy "source_group", ka¿dy nag³ówek/Ÿród³o musi do którejœ
# trafiæ; w przeciwnym wypadku CMake generuje b³êdny projekt

#------------------------------------------------------------------------------

# nag³ówki prywatne
set(PRIVATE_H
)
#source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRIVATE_H})

# nag³ówki publiczne
#message("string(REPLACE ${EDR_ROOT} ${EDR_INCLUDE_ROOT} HEADER_PATH ${PROJECT_SOURCE_DIR})")
#string(REPLACE ${EDR_ROOT} ${EDR_INCLUDE_ROOT} HEADER_PATH ${PROJECT_SOURCE_DIR})
set(HEADER_PATH ${EDR_INCLUDE_ROOT}/core)
set(PUBLIC_H
	${HEADER_PATH}/GridModel.h
	${HEADER_PATH}/OsgWidgetGridView.h
	${HEADER_PATH}/OsgWidgetBorder.h
	${HEADER_PATH}/Buttonized.h
	${HEADER_PATH}/Borderized.h
	${HEADER_PATH}/LabeledView.h
	${HEADER_PATH}/Placeable.h
	${HEADER_PATH}/Grid.h
	${HEADER_PATH}/AspectRatioKeeper.h
	${HEADER_PATH}/MultiView.h
	${HEADER_PATH}/MultiViewWidgetItem.h
	${HEADER_PATH}/EmbeddedWindow.h
	${HEADER_PATH}/MultiViewFunctorWidgetAdapter.h
	${HEADER_PATH}/MultiViewCameraItem.h
	${HEADER_PATH}/MultiViewManipulatorItem.h
	${HEADER_PATH}/OsgSceneDump.h
)
source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES ${PUBLIC_H})
set(TARGET_H ${PRIVATE_H} ${PUBLIC_H})

#------------------------------------------------------------------------------

# Ÿród³a
set(TARGET_SRC
	ASFAMCData.cpp
	ASFAMCParser.cpp
	QOSGWidget.cpp
	GridModel.cpp
	OsgWidgetGridView.cpp
	OsgWidgetBorder.cpp
	Window.cpp
	Grid.cpp
	AspectRatioKeeper.cpp
	MultiView.cpp
	MultiViewWidgetItem.cpp
	Chart.cpp
	c3dParser.cpp
	C3D_Data.cpp
	ChartData.cpp
	Data.cpp
	DateTime.cpp
	DECreader.cpp
	Filesystem.cpp
	GroupData.cpp
	Header.cpp
	INTELreader.cpp
	LineChart.cpp
	MIPSreader.cpp
	Parameter.cpp
	Parameters.cpp
	ChartPointer.cpp
	EmbeddedWindow.cpp
	MultiViewFunctorWidgetAdapter.cpp
	Quaternioncal.cpp
	MultiViewCameraItem.cpp
	MultiViewManipulatorItem.cpp
	OsgSceneDump.cpp
)
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_SRC})

# metadane generowane przez MOC
SET(TARGET_MOC_H
)
#QT4_WRAP_CPP(TARGET_MOC_SRC ${TARGET_MOC_H})
#source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_MOC_SRC})
set(TARGET_SRC ${TARGET_SRC} ${TARGET_MOC_SRC})

#------------------------------------------------------------------------------
# UI
set(TARGET_UI
)
#QT4_WRAP_UI(TARGET_UI_H ${TARGET_UI})
#source_group("${SOURCEGROUP_UI}" FILES ${TARGET_UI_H} ${TARGET_UI})
set(TARGET_H ${TARGET_H} ${TARGET_UI_H} ${TARGET_UI})

#------------------------------------------------------------------------------
# CONFIG
set(CONFIG_H
)
#source_group("${SOURCEGROUP_PRIVATE_HEADERS}\\config" FILES ${CONFIG_H})
set(TARGET_H ${TARGET_H} ${CONFIG_H})

set(CONFIG_SRC
)
#source_group("${SOURCEGROUP_SOURCES}\\config" FILES ${CONFIG_SRC})
set(TARGET_SRC ${TARGET_SRC} ${CONFIG_SRC})

#------------------------------------------------------------------------------
# prekompilowany nag³ówek
set(PRECOMPILED_H CorePCH.h)
set(PRECOMPILED_SRC CorePCH.cpp)
SET_PRECOMPILED_HEADER(${PRECOMPILED_H} ${PRECOMPILED_SRC} "${TARGET_SRC}")
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRECOMPILED_H})
set(TARGET_H ${TARGET_H} ${PRECOMPILED_H})
source_group("${SOURCEGROUP_SOURCES}" FILES ${PRECOMPILED_SRC})
set(TARGET_SRC ${TARGET_SRC} ${PRECOMPILED_SRC})

#------------------------------------------------------------------------------
# tworzenie

# obsluga OpenGL
set(QT_USE_QTOPENGL TRUE) 
include(${QT_USE_FILE})

# kompilacja do wspó³dzielonej biblioteki
add_library(${TARGET_TARGETNAME} STATIC ${TARGET_SRC} ${TARGET_H})
# nag³ówki
include_directories(${DEFAULT_PROJECT_INCLUDES})
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	

ON_PROJECT_ADDED(corelib)
