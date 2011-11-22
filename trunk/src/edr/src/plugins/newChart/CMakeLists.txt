project(newChart)
set(TARGET_TARGETNAME plugin_newChart)

# obs³uga QT
set(QT_USE_QTOPENGL TRUE)
include(${QT_USE_FILE})

#------------------------------------------------------------------------------

# nag³ówki
string(REPLACE ${PROJECT_ROOT} ${PROJECT_INCLUDE_ROOT} HEADER_PATH ${PROJECT_SOURCE_DIR})
set(PUBLIC_H
)
source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES ${PUBLIC_H})
set(PRIVATE_H
	NewChartVisualizer.h
	NewChartSerie.h
	NewChartPicker.h
)
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRIVATE_H})
set(TARGET_H ${PRIVATE_H} ${PUBLIC_H})

#------------------------------------------------------------------------------

# Ÿród³a
set(TARGET_SRC
	NewChartPlugin.cpp	
	NewChartVisualizer.cpp
	NewChartSerie.cpp
	NewChartPicker.cpp
)
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_SRC})

SET(TARGET_MOC_H
	NewChartVisualizer.h
	NewChartPicker.h
)
QT4_WRAP_CPP(TARGET_MOC_SRC ${TARGET_MOC_H})
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_MOC_SRC})
set(TARGET_SRC ${TARGET_SRC} ${TARGET_MOC_SRC})


#------------------------------------------------------------------------------
# UI
SET(TARGET_UI
#pusto
)
QT4_WRAP_UI(TARGET_UI_H ${TARGET_UI})
source_group("${SOURCEGROUP_UI}" FILES ${TARGET_UI_H} ${TARGET_UI})
set(TARGET_H ${TARGET_H} ${TARGET_UI} ${TARGET_UI_H})


#------------------------------------------------------------------------------
# prekompilowany nag³ówek
set(PRECOMPILED_H NewChartPCH.h)
set(PRECOMPILED_SRC NewChartPCH.cpp)
SET_PRECOMPILED_HEADER(${PRECOMPILED_H} ${PRECOMPILED_SRC} "${TARGET_SRC}")
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRECOMPILED_H})
set(TARGET_H ${TARGET_H} ${PRECOMPILED_H})
source_group("${SOURCEGROUP_SOURCES}" FILES ${PRECOMPILED_SRC})
set(TARGET_SRC ${TARGET_SRC} ${PRECOMPILED_SRC})

#------------------------------------------------------------------------------
# tworzenie

# kompilacja do wspó³dzielonej biblioteki
add_library(${TARGET_TARGETNAME} SHARED ${TARGET_SRC} ${TARGET_H})
#instalacja
install(TARGETS ${TARGET_TARGETNAME} RUNTIME DESTINATION bin COMPONENT newChart)
#add_executable(${TARGET_TARGETNAME} ${TARGET_SRC} ${TARGET_H})
# nag³ówki
include_directories( ${DEFAULT_PROJECT_INCLUDES} )
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	

# ¿eby mo¿na by³o u¿yæ tej biblioteki w makrach
ON_PROJECT_ADDED(${TARGET_TARGETNAME})