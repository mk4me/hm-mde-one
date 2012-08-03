project(dataExplorer)
# nazwa
set(TARGET_TARGETNAME
	plugin_dataExplorer
)

#------------------------------------------------------------------------------

# nag³ówki prywatne
set(PRIVATE_H
	DataExplorerPlugin.h
	DataExplorer.h
	DataExplorerService.h
	DataExplorerWidget.h
)
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRIVATE_H})

# nag³ówki publiczne
string(REPLACE ${PROJECT_ROOT} ${PROJECT_INCLUDE_ROOT} HEADER_PATH ${PROJECT_SOURCE_DIR})
set(PUBLIC_H
	${HEADER_PATH}/IDataExplorerSource.h
)
source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES ${PUBLIC_H})
set(TARGET_H ${PRIVATE_H} ${PUBLIC_H})

#------------------------------------------------------------------------------

# Ÿród³a
set(TARGET_SRC
	DataExplorerPlugin.cpp
	DataExplorer.cpp
	DataExplorerService.cpp
	DataExplorerWidget.cpp
)
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_SRC})

# metadane generowane przez MOC
SET(TARGET_MOC_H
	DataExplorerWidget.h
	DataExplorerService.h
)

QT4_WRAP_CPP(TARGET_MOC_SRC ${TARGET_MOC_H})
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_MOC_SRC})
set(TARGET_SRC ${TARGET_SRC} ${TARGET_MOC_SRC})

#------------------------------------------------------------------------------
# UI
set(TARGET_UI
	ui/DataExplorerWidget.ui
)

QT4_WRAP_UI(TARGET_UI_H ${TARGET_UI})
source_group("${SOURCEGROUP_UI}" FILES ${TARGET_UI_H} ${TARGET_UI})
set(TARGET_H ${TARGET_H} ${TARGET_UI_H} ${TARGET_UI})


#------------------------------------------------------------------------------
# prekompilowany nag³ówek
set(PRECOMPILED_H DataExplorerPCH.h)
set(PRECOMPILED_SRC DataExplorerPCH.cpp)
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
add_library(${TARGET_TARGETNAME} SHARED ${TARGET_SRC} ${TARGET_H})
#instalacja
install(FILES ${PUBLIC_H} DESTINATION include/plugins/${PROJECT_NAME} COMPONENT ${PROJECT_NAME}_dev)
if (WIN32)
install(TARGETS ${TARGET_TARGETNAME} RUNTIME DESTINATION bin COMPONENT ${PROJECT_NAME})
endif()
install(TARGETS ${TARGET_TARGETNAME} LIBRARY DESTINATION lib/plugins/${PROJECT_NAME} ARCHIVE DESTINATION lib/plugins/${PROJECT_NAME} COMPONENT ${PROJECT_NAME}_dev)
# nag³ówki
include_directories(${DEFAULT_PROJECT_INCLUDES})
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	

ON_PROJECT_ADDED(${TARGET_TARGETNAME})
