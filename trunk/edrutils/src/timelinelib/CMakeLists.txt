project(timeline)
# nazwa
set(TARGET_TARGETNAME
	timeline
)

#------------------------------------------------------------------------------

# nag³ówki prywatne
set(PRIVATE_H
)

# source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRIVATE_H})

# nag³ówki publiczne
string(REPLACE ${PROJECT_ROOT} ${PROJECT_INCLUDE_ROOT} HEADER_PATH ${PROJECT_SOURCE_DIR})
set(PUBLIC_H
	${HEADER_PATH}/Controller.h
	${HEADER_PATH}/ISelection.h
	${HEADER_PATH}/IStream.h
	${HEADER_PATH}/Model.h
	${HEADER_PATH}/SimpleSelection.h
	${HEADER_PATH}/State.h
	${HEADER_PATH}/Stream.h
	${HEADER_PATH}/Tag.h
	${HEADER_PATH}/TagSelection.h
	${HEADER_PATH}/TagType.h
	${HEADER_PATH}/Types.h
	${HEADER_PATH}/View.h
)

source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES ${PUBLIC_H})
set(TARGET_H ${PRIVATE_H} ${PUBLIC_H})

#------------------------------------------------------------------------------

# Ÿród³a
set(TARGET_SRC
	Controller.cpp
	ISelection.cpp
	IStream.cpp
	Model.cpp
	SimpleSelection.cpp
	State.cpp
	Stream.cpp
	Tag.cpp
	TagSelection.cpp
	TagType.cpp
	View.cpp
)
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_SRC})

#------------------------------------------------------------------------------

# kompilacja do wspó³dzielonej biblioteki
add_library(${TARGET_TARGETNAME} STATIC ${TARGET_SRC} ${TARGET_H})
# nag³ówki
include_directories( ${DEFAULT_PROJECT_INCLUDES} )
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	
# nazwa
set_target_properties(${TARGET_TARGETNAME} PROPERTIES PROJECT_LABEL "Library ${TARGET_TARGETNAME}")

# ¿eby mo¿na by³o u¿yæ tej biblioteki w makrach
ON_PROJECT_ADDED(${TARGET_TARGETNAME})

#instalacja
install(TARGETS ${TARGET_TARGETNAME} DESTINATION lib)
install(FILES ${PUBLIC_H} DESTINATION include/${TARGET_TARGETNAME})