project(osgui)
# nazwa
set(TARGET_TARGETNAME
	osgui
)

#------------------------------------------------------------------------------

# nag³ówki prywatne
set(PRIVATE_H
)
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRIVATE_H})

# nag³ówki publiczne
string(REPLACE ${PROJECT_ROOT} ${PROJECT_INCLUDE_ROOT} HEADER_PATH ${PROJECT_SOURCE_DIR})
set(PUBLIC_H
	${HEADER_PATH}/AspectRatioKeeper.h
	${HEADER_PATH}/Borderized.h
	${HEADER_PATH}/Buttonized.h
	${HEADER_PATH}/ContextMenu.h
	${HEADER_PATH}/EmbeddedWindow.h
	${HEADER_PATH}/Grid.h
	${HEADER_PATH}/KeyboardMapper.h
	${HEADER_PATH}/KeyboardMapperHandler.h
	${HEADER_PATH}/LabeledView.h
	${HEADER_PATH}/OsgWidgetUtils.h
	${HEADER_PATH}/DisableableRenderer.h
	${HEADER_PATH}/QOsgWidgets.h
	${HEADER_PATH}/QOsgWidgetsDeprecated.h
	${HEADER_PATH}/StyleEx.h
	${HEADER_PATH}/Toolbar.h
	${HEADER_PATH}/Tooltip.h
	${HEADER_PATH}/TooltipHost.h
	${HEADER_PATH}/Utils2D.h	
	${HEADER_PATH}/EventCallback.h	
	${HEADER_PATH}/macroHacks.h	
)
source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES ${PUBLIC_H})
set(TARGET_H ${PRIVATE_H} ${PUBLIC_H})

#------------------------------------------------------------------------------
# Ÿród³a
set(TARGET_SRC
	AspectRatioKeeper.cpp
	ContextMenu.cpp
	DisableableRenderer.cpp
	EmbeddedWindow.cpp
	Grid.cpp
	KeyboardMapper.cpp
	KeyboardMapperHandler.cpp
	OsgWidgetUtils.cpp
	QOsgWidgets.cpp
	QOsgWidgetsDeprecated.cpp
	StyleEx.cpp
	Toolbar.cpp
	Tooltip.cpp
	TooltipHost.cpp
	Utils2D.cpp
)
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_SRC})

#------------------------------------------------------------------------------
# prekompilowany nag³ówek
set(PRECOMPILED_H PCH.h)
set(PRECOMPILED_SRC PCH.cpp)
SET_PRECOMPILED_HEADER(${PRECOMPILED_H} ${PRECOMPILED_SRC} "${TARGET_SRC}")
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRECOMPILED_H})
set(TARGET_H ${TARGET_H} ${PRECOMPILED_H})
source_group("${SOURCEGROUP_SOURCES}" FILES ${PRECOMPILED_SRC})
set(TARGET_SRC ${TARGET_SRC} ${PRECOMPILED_SRC})

#------------------------------------------------------------------------------
# tworzenie

# kompilacja do wspó³dzielonej biblioteki
add_library(${TARGET_TARGETNAME} STATIC ${TARGET_SRC} ${TARGET_H})

#instalacja
install(TARGETS ${TARGET_TARGETNAME} LIBRARY DESTINATION lib/${PROJECT_NAME} ARCHIVE DESTINATION lib/${PROJECT_NAME} COMPONENT ${PROJECT_NAME}_dev )
install(FILES ${PUBLIC_H} DESTINATION include/${PROJECT_NAME} COMPONENT ${PROJECT_NAME}_dev)
# nag³ówki
include_directories(${DEFAULT_PROJECT_INCLUDES})
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	
# nazwa
set_target_properties(${TARGET_TARGETNAME} PROPERTIES PROJECT_LABEL "Library ${TARGET_TARGETNAME}")

ON_PROJECT_ADDED(${TARGET_TARGETNAME})
