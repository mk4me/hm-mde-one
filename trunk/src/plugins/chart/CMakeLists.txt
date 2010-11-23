project(chart)
set(TARGET_TARGETNAME chart)

# obs³uga QT
set(QT_USE_QTOPENGL TRUE)
include(${QT_USE_FILE})

add_definitions(-D__LIBRARY_DYNAMIC__)

#------------------------------------------------------------------------------

# nag³ówki
string(REPLACE ${EDR_ROOT} ${EDR_INCLUDE_ROOT} HEADER_PATH ${PROJECT_SOURCE_DIR})
set(PUBLIC_H

		LineChart.h
		ChartData.h
		Pointer.h
		Header.h
		INTELreader.h
		MIPSreader.h
		Parameter.h
		Parameters.h
		ProcessorReader.h
		Data.h
		DECreader.h

)
source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES ${PUBLIC_H})
set(PRIVATE_H
	
	ChartPlugin.h
	ChartService.h
	ChartWidget.h
	IChart.h
	ChartDataSeries.h
	GroupData.h
	ChartViewer.h
	C3DChartData.h
	EMGService.h
	GRFService.h
)
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRIVATE_H})
set(TARGET_H ${PRIVATE_H} ${PUBLIC_H})

#------------------------------------------------------------------------------

# Ÿród³a
set(TARGET_SRC
	main.cpp
	ChartPlugin.cpp
	ChartWidget.cpp
	ChartService.cpp
	ChartDataSeries.cpp
	ChartData.cpp
	LineChart.cpp
	Pointer.cpp
	

	Data.cpp
	DECreader.cpp
	GroupData.cpp
	Header.cpp
	INTELreader.cpp
	MIPSreader.cpp
	Parameter.cpp
	Parameters.cpp
	ChartViewer.cpp
	C3DChartData.cpp
	EMGService.cpp
	GRFService.cpp
)
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_SRC})

SET(TARGET_MOC_H
ChartWidget.h
)
QT4_WRAP_CPP(TARGET_MOC_SRC ${TARGET_MOC_H})
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_MOC_SRC})
set(TARGET_SRC ${TARGET_SRC} ${TARGET_MOC_SRC})


#------------------------------------------------------------------------------
# UI
SET(TARGET_UI
	ui/ChartWidget.ui
)
QT4_WRAP_UI(TARGET_UI_H ${TARGET_UI})
source_group("${SOURCEGROUP_UI}" FILES ${TARGET_UI_H} ${TARGET_UI})
set(TARGET_H ${TARGET_H} ${TARGET_UI} ${TARGET_UI_H})


#------------------------------------------------------------------------------
# prekompilowany nag³ówek
set(PRECOMPILED_H ChartPCH.h)
set(PRECOMPILED_SRC ChartPCH.cpp)
SET_PRECOMPILED_HEADER(${PRECOMPILED_H} ${PRECOMPILED_SRC} "${TARGET_SRC}")
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRECOMPILED_H})
set(TARGET_H ${TARGET_H} ${PRECOMPILED_H})
source_group("${SOURCEGROUP_SOURCES}" FILES ${PRECOMPILED_SRC})
set(TARGET_SRC ${TARGET_SRC} ${PRECOMPILED_SRC})

#------------------------------------------------------------------------------
# tworzenie

# kompilacja do wspó³dzielonej biblioteki
add_library(${TARGET_TARGETNAME} SHARED ${TARGET_SRC} ${TARGET_H})
#add_executable(${TARGET_TARGETNAME} ${TARGET_SRC} ${TARGET_H})
# nag³ówki
include_directories( ${DEFAULT_PROJECT_INCLUDES} )
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	

# ¿eby mo¿na by³o u¿yæ tej biblioteki w makrach
ON_PROJECT_ADDED(${TARGET_TARGETNAME})