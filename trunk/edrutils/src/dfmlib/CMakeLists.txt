project(dfmlib)
# nazwa
set(TARGET_TARGETNAME
	dfmlib
)

#------------------------------------------------------------------------------

# nag³ówki prywatne
set(PRIVATE_H
)

# source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRIVATE_H})

# nag³ówki publiczne
string(REPLACE ${PROJECT_ROOT} ${PROJECT_INCLUDE_ROOT} HEADER_PATH ${PROJECT_SOURCE_DIR})
set(PUBLIC_H
	${HEADER_PATH}/DFLMTypes.h
	${HEADER_PATH}/Connection.h
	${HEADER_PATH}/DFInterface.h
	${HEADER_PATH}/DFModel.h
	${HEADER_PATH}/DFNode.h
	${HEADER_PATH}/DFPin.h
	${HEADER_PATH}/DFSourceNode.h
	${HEADER_PATH}/Model.h
	${HEADER_PATH}/Node.h
	${HEADER_PATH}/Pin.h
)
source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES ${PUBLIC_H})
set(TARGET_H ${PRIVATE_H} ${PUBLIC_H})

#------------------------------------------------------------------------------

# Ÿród³a
set(TARGET_SRC
	DFModel.cpp
	DFNode.cpp
	DFPin.cpp
	DFSourceNode.cpp
	DFInterface.cpp
	Model.cpp
	Node.cpp
	Pin.cpp
	Connection.cpp
)
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_SRC})

#------------------------------------------------------------------------------

# kompilacja do wspó³dzielonej biblioteki
add_library(${TARGET_TARGETNAME} STATIC ${TARGET_SRC} ${TARGET_H})
# nag³ówki
include_directories( ${DEFAULT_PROJECT_INCLUDES} )
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	

# ¿eby mo¿na by³o u¿yæ tej biblioteki w makrach
ON_PROJECT_ADDED(${TARGET_TARGETNAME})

#instalacja
install(TARGETS ${TARGET_TARGETNAME} DESTINATION lib)
install(FILES ${PUBLIC_H} DESTINATION include/${TARGET_TARGETNAME})