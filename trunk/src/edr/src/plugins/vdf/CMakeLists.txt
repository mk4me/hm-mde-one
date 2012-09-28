project(vdf)
set(TARGET_TARGETNAME plugin_vdf)

# obs³uga QT
set(QT_USE_QTOPENGL TRUE)
include(${QT_USE_FILE})

# add_definitions(-D__LIBRARY_DYNAMIC__)

#------------------------------------------------------------------------------

# nag³ówki
string(REPLACE ${PROJECT_ROOT} ${PROJECT_INCLUDE_ROOT} HEADER_PATH ${PROJECT_SOURCE_DIR})
set(PUBLIC_H
	${HEADER_PATH}/VdfService.h
)
source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES ${PUBLIC_H})
set(PRIVATE_H
	VdfWidget.h
	VdfQtModel.h
	DeleteConnectionContextMenu.h
	VdfCustomQOsgWidget.h
	EDRPin.h
	EDRSourceNode.h
	EDRProcessingNode.h
	EDRNodesFactory.h
)
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRIVATE_H})
set(TARGET_H ${PRIVATE_H} ${PUBLIC_H})

#------------------------------------------------------------------------------

# Ÿród³a
set(TARGET_SRC
	VdfService.cpp
	VdfPlugin.cpp
	VdfWidget.cpp
	VdfQtModel.cpp
	DeleteConnectionContextMenu.cpp
	VdfCustomQOsgWidget.cpp
	EDRPin.cpp
	EDRSourceNode.cpp
	EDRProcessingNode.cpp
	EDRNodesFactory.cpp
)
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_SRC})

SET(TARGET_MOC_H
	VdfWidget.h
	DeleteConnectionContextMenu.h
	VdfQtModel.h
	VdfCustomQOsgWidget.h
)
QT4_WRAP_CPP(TARGET_MOC_SRC ${TARGET_MOC_H})
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_MOC_SRC})
set(TARGET_SRC ${TARGET_SRC} ${TARGET_MOC_SRC})

#------------------------------------------------------------------------------
# UI
SET(TARGET_UI
	ui/VdfWidget.ui
)
QT4_WRAP_UI(TARGET_UI_H ${TARGET_UI})
source_group("${SOURCEGROUP_UI}" FILES ${TARGET_UI_H} ${TARGET_UI})
set(TARGET_H ${TARGET_H} ${TARGET_UI} ${TARGET_UI_H})


#------------------------------------------------------------------------------
# prekompilowany nag³ówek
set(PRECOMPILED_H VdfPCH.h)
set(PRECOMPILED_SRC VdfPCH.cpp)
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
install(FILES ${PUBLIC_H} DESTINATION include/plugins/${PROJECT_NAME} COMPONENT ${PROJECT_NAME}_dev)
if (WIN32)
install(TARGETS ${TARGET_TARGETNAME} RUNTIME DESTINATION bin COMPONENT ${PROJECT_NAME})
endif()
install(TARGETS ${TARGET_TARGETNAME} LIBRARY DESTINATION lib/plugins/${PROJECT_NAME} ARCHIVE DESTINATION lib/plugins/${PROJECT_NAME} COMPONENT ${PROJECT_NAME}_dev)
# nag³ówki
include_directories( ${DEFAULT_PROJECT_INCLUDES} )
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	

# ¿eby mo¿na by³o u¿yæ tej biblioteki w makrach
ON_PROJECT_ADDED(${TARGET_TARGETNAME})
