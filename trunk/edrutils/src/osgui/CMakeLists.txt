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
	${HEADER_PATH}/EmbeddedWindow.h
	${HEADER_PATH}/Grid.h
	${HEADER_PATH}/LabeledView.h
	${HEADER_PATH}/OsgWidgetUtils.h
	${HEADER_PATH}/DisableableRenderer.h
	${HEADER_PATH}/QOsgWidgets.h
	${HEADER_PATH}/QOsgWidgetsDeprecated.h
	
)
source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES ${PUBLIC_H})
set(TARGET_H ${PRIVATE_H} ${PUBLIC_H})

#------------------------------------------------------------------------------
# Ÿród³a
set(TARGET_SRC
	AspectRatioKeeper.cpp
	DisableableRenderer.cpp
	EmbeddedWindow.cpp
	Grid.cpp
	OsgWidgetUtils.cpp
	QOsgWidgets.cpp
	QOsgWidgetsDeprecated.cpp
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
install(TARGETS ${TARGET_TARGETNAME} RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib COMPONENT vidlib)
install(FILES ${PUBLIC_H} DESTINATION include/${TARGET_TARGETNAME})
# nag³ówki
include_directories(${DEFAULT_PROJECT_INCLUDES})
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	

ON_PROJECT_ADDED(${TARGET_TARGETNAME})
